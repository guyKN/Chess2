//
// Created by guykn on 12/7/2020.
//
#include <stdexcept>
#include "ChessBoard.h"
#include "iostream"
#include "Bitboards.h"

namespace Chess {

    ZobristData zobristData{};

    void ZobristData::init() {
        PRNG rng{1070372};
        if (!initialized) {
            for (Square square = SQ_FIRST; square <= SQ_LAST; ++square) {
                for (Piece piece = PIECE_FIRST; piece <= PIECE_LAST_NOT_EMPTY; ++piece) {
                    pieceKeys[square][piece] = rng.rand<Key>();
                }
            }
            for (auto &castlingRight : castlingRightKeys) {
                castlingRight = rng.rand<Key>();
            }
            for (File file = FILE_FIRST; file <= FILE_LAST; ++file) {
                enPassantFiles[file] = rng.rand<Key>();
            }
            blackToMove = rng.rand<Key>();
            initialized = true;
        }
    }

    std::ostream &operator<<(ostream &os, MoveGenType moveGenType) {
        const static string moveGenTypeToString[] = {"ALL", "CAPTURES", "NON_CAPTURES"};
        return os << moveGenTypeToString[moveGenType];
    }

    bool ThreatData::isEmpty() {
        if (threatsByPlayer[WHITE] || threatsByPlayer[BLACK] || _isInCheck[WHITE] || _isInCheck[BLACK] ||
            _isInDoubleCheck[WHITE] || _isInDoubleCheck[BLACK] ||
            kingBlockers[WHITE] || kingBlockers[BLACK] || (checkEvasionSquares != BITBOARD_FULL)) {
            return false;
        }
        for (Bitboard& threats:threatsByPiece){
            if(threats){
                return false;
            }
        }
        return true;
    }


    const Piece ChessBoard::startingBoard[] = {PIECE_WHITE_ROOK, PIECE_WHITE_KNIGHT, PIECE_WHITE_BISHOP,
                                               PIECE_WHITE_QUEEN, PIECE_WHITE_KING, PIECE_WHITE_BISHOP,
                                               PIECE_WHITE_KNIGHT, PIECE_WHITE_ROOK,
                                               PIECE_WHITE_PAWN, PIECE_WHITE_PAWN, PIECE_WHITE_PAWN, PIECE_WHITE_PAWN,
                                               PIECE_WHITE_PAWN, PIECE_WHITE_PAWN, PIECE_WHITE_PAWN, PIECE_WHITE_PAWN,
                                               PIECE_NONE, PIECE_NONE, PIECE_NONE, PIECE_NONE, PIECE_NONE, PIECE_NONE,
                                               PIECE_NONE, PIECE_NONE,
                                               PIECE_NONE, PIECE_NONE, PIECE_NONE, PIECE_NONE, PIECE_NONE, PIECE_NONE,
                                               PIECE_NONE, PIECE_NONE,
                                               PIECE_NONE, PIECE_NONE, PIECE_NONE, PIECE_NONE, PIECE_NONE, PIECE_NONE,
                                               PIECE_NONE, PIECE_NONE,
                                               PIECE_NONE, PIECE_NONE, PIECE_NONE, PIECE_NONE, PIECE_NONE, PIECE_NONE,
                                               PIECE_NONE, PIECE_NONE,
                                               PIECE_BLACK_PAWN, PIECE_BLACK_PAWN, PIECE_BLACK_PAWN, PIECE_BLACK_PAWN,
                                               PIECE_BLACK_PAWN, PIECE_BLACK_PAWN, PIECE_BLACK_PAWN, PIECE_BLACK_PAWN,
                                               PIECE_BLACK_ROOK, PIECE_BLACK_KNIGHT, PIECE_BLACK_BISHOP,
                                               PIECE_BLACK_QUEEN, PIECE_BLACK_KING, PIECE_BLACK_BISHOP,
                                               PIECE_BLACK_KNIGHT, PIECE_BLACK_ROOK};


    void ChessBoard::setPosition(const Piece *piecesArray, Player currentPlayer) {

        this->currentPlayer = currentPlayer;
        enPassantSquare = SQ_INVALID;

        castlingRights = CASTLE_RIGHTS_ALL;

        for (Square square = SQ_FIRST; square <= SQ_LAST; ++square) {
            Piece piece = piecesArray[square];
            pieceOn(square) = piece;
        }
        updateBitboards();
        initHashKey();
    }

    std::ostream &operator<<(std::ostream &os, const ChessBoard &chessBoard) {
        os << "\n  +---+---+---+---+---+---+---+---+\n";
        for (Rank rank = RANK_FIRST; rank <= RANK_LAST; ++rank) {
            os << (1 + rank) << " ";
            for (File file = FILE_FIRST; file <= FILE_LAST; ++file) {
                Piece piece = chessBoard.getPieceOn(makeSquare(rank, file));
                os << "| " << toChar(piece) << " ";
            }
            os << "| " << "\n  +---+---+---+---+---+---+---+---+\n";
        }
        os << "    A   B   C   D   E   F   G   H\n";
        return os;
    }

    //todo: optimize templates, see if there's a way to make more stuff inline.

    void ChessBoard::printBitboards() const {
        if (threatData != nullptr) {
            cout << "\n" << (threatData->isInCheck(currentPlayer) ? "Check" : "Not Check");

            cout << "\n" << (threatData->isInDoubleCheck(currentPlayer) ? "Double Check" : "Not Double Check");

            cout << "\nPinned: ";

            printBitboard(threatData->kingBlockersOf(currentPlayer));

            cout << "\nCheckEvasionsSquares: ";

            printBitboard(threatData->checkEvasionSquares);
        } else {
            cout << "threatData == nullptr\n";
        }

        cout << "\nCurrent player: " << currentPlayer;

        cout << "\nen passant square: " << toString(enPassantSquare);

        cout << "\nWhite Pawns:";
        printBitboard(bitboardOf(PIECE_WHITE_PAWN));

        cout << "\nWhite Knights";
        printBitboard(bitboardOf(PIECE_WHITE_KNIGHT));

        cout << "\nWhite Bishops";
        printBitboard(bitboardOf(PIECE_WHITE_BISHOP));

        cout << "\nWhite Rook";
        printBitboard(bitboardOf(PIECE_WHITE_ROOK));

        cout << "\nWhite Queen";
        printBitboard(bitboardOf(PIECE_WHITE_QUEEN));

        cout << "\nWhite King";
        printBitboard(bitboardOf(PIECE_WHITE_KING));

        cout << "\nBlack Pawns:";
        printBitboard(bitboardOf(PIECE_BLACK_PAWN));

        cout << "\nBlack Knights";
        printBitboard(bitboardOf(PIECE_BLACK_KNIGHT));

        cout << "\nBlack Bishops";
        printBitboard(bitboardOf(PIECE_BLACK_BISHOP));

        cout << "\nBlack Rook";
        printBitboard(bitboardOf(PIECE_BLACK_ROOK));

        cout << "\nBlack Queen";
        printBitboard(bitboardOf(PIECE_BLACK_QUEEN));

        cout << "\nBlack King";
        printBitboard(bitboardOf(PIECE_BLACK_KING));

        cout << "\nEmpty squares:";
        printBitboard(bitboardOf(PIECE_NONE));

        cout << "\nWhite Pieces:";
        printBitboard(bitboardOf(WHITE));

        cout << "\nBlack Pieces:";
        printBitboard(bitboardOf(BLACK));

        cout << "\nFen String: \n";
        getFen(cout);

        cout << *this;
    }

    //todo: make this not run in release mode
    void ChessBoard::assertOk() const {
#if FOR_RELEASE == 0
        if (!isOk()) {
            printBitboards();
            assert(false);
        }
#endif
    }

    void ChessBoard::setGameHistory(GameHistory &gameHistory) {
        for (Move move: gameHistory.moves) {
            doGameMove(move);
        }
    }

    MoveRevertData ChessBoard::doMove(Move move) {

        //todo: check if it's faster to call src() dst() once
        // todo: use xor in order to do moves faster (can place and remove pieces with one bb)
        /// https://www.chessprogramming.org/General_Setwise_Operations#UpdateByMove
        assert(move.isOk());
        Piece srcPiece = pieceOn(move.src());
        Piece dstPiece = pieceOn(move.dst());

        MoveRevertData moveRevertData = getMoveRevertData(dstPiece);

        if (enPassantSquare != SQ_INVALID) {
            hashKey ^= zobristData.keyOf(fileOf(enPassantSquare));
        }

        removePieceFrom(move.src(), srcPiece, currentPlayer);

        switch (move.moveType()) {
            case Move::EN_PASSANT_MOVE:
                if (move.isEnPassantCapture()) {
                    Piece enemyPawn = makePiece(PIECE_TYPE_PAWN, ~currentPlayer);
                    removePieceFrom(enPassantSquare, enemyPawn, ~currentPlayer);
                    enPassantSquare = SQ_INVALID;
                } else {
                    // pawn forward 2
                    hashKey ^= zobristData.keyOf(fileOf(move.dst()));
                    enPassantSquare = move.dst();
                }
                placePieceOn(move.dst(), srcPiece, currentPlayer);
                break;
            case Move::PROMOTION_MOVE:
                setPieceOn(move.dst(),
                           dstPiece,
                           makePiece(move.promotionPieceType(), currentPlayer),
                           currentPlayer);
                enPassantSquare = SQ_INVALID;
                break;
            case Move::NORMAL_MOVE:
                setPieceOn(move.dst(),
                           dstPiece,
                           srcPiece,
                           currentPlayer);
                enPassantSquare = SQ_INVALID;
                break;
            case Move::CASTLING_MOVE:
                Piece rook = makePiece(PIECE_TYPE_ROOK, currentPlayer);
                const CastlingData &castlingData = CastlingData::fromCastlingType(move.castlingType());
                placePieceOn(move.dst(), srcPiece, currentPlayer);
                removePieceFrom(castlingData.rookSrc, rook, currentPlayer);
                placePieceOn(castlingData.rookDst, rook, currentPlayer);
                enPassantSquare = SQ_INVALID;
                break;
        }

        updateCastling(move);
        swapPlayer();
        return moveRevertData;
    }

    void ChessBoard::undoMove(Move move, MoveRevertData &moveRevertData) {
        //todo: should pass bestMove_ by refrence or value
        revertTo(moveRevertData);
        Player opponent = ~currentPlayer;
        //todo: see if refrence is faster than direct value
        Piece currentPiece = pieceOn(move.dst());
        Piece capturedPiece = moveRevertData.capturedPiece;
        setPieceOn2(move.dst(), currentPiece, opponent, capturedPiece);
        switch (move.moveType()) {
            case Move::EN_PASSANT_MOVE:
                if (move.isEnPassantCapture()) {
                    Piece pawn = makePiece(PIECE_TYPE_PAWN, currentPlayer);
                    Square prevEnPassantSquare = move.dst() + 8 * directionOf(currentPlayer);
                    placePieceOn(prevEnPassantSquare, pawn, currentPlayer);
                } else {
                    //pawn forward 2, nothing to do here
                }
                placePieceOn(move.src(), currentPiece, opponent);
                break;
            case Move::PROMOTION_MOVE:
                placePieceOn(move.src(), makePiece(PIECE_TYPE_PAWN, opponent), opponent);
                break;
            case Move::NORMAL_MOVE:
                placePieceOn(move.src(), currentPiece, opponent);
                break;
            case Move::CASTLING_MOVE:
                placePieceOn(move.src(), currentPiece, opponent);
                Piece rook = makePiece(PIECE_TYPE_ROOK, opponent);
                const CastlingData &castlingData = CastlingData::fromCastlingType(move.castlingType());
                removePieceFrom(castlingData.rookDst, rook, opponent);
                placePieceOn(castlingData.rookSrc, rook, opponent);
                break;
        }
        swapPlayer(); //optimize: bestMove_ to front of fuction, replace opponent with currentPlayer
    }

    void ChessBoard::revertTo(const MoveRevertData &moveRevertData) {
        hashKey ^= zobristData.keyOf(castlingRights);
        castlingRights = moveRevertData.castlingRights;
        hashKey ^= zobristData.keyOf(castlingRights);

        if (enPassantSquare != SQ_INVALID) {
            File file = fileOf(enPassantSquare);
            hashKey ^= zobristData.keyOf(file);
        }
        if (moveRevertData.enPassantSquare != SQ_INVALID) {
            File file = fileOf(moveRevertData.enPassantSquare);
            hashKey ^= zobristData.keyOf(file);
        }
        enPassantSquare = moveRevertData.enPassantSquare;
    }

    //todo: should this be inline?
    //todo: pass src and dst directly, or just pass a mask
    void ChessBoard::updateCastling(const Move &move) {
        Bitboard moveSquares = maskOf(move.src()) | maskOf(move.dst());
        hashKey ^= zobristData.keyOf(castlingRights);
        CastlingData::updateCastlingRights(moveSquares, castlingRights);
        hashKey ^= zobristData.keyOf(castlingRights);
    }

    //todo: only call this when assertions are enabled
    bool ChessBoard::isOk() const {
        bool noPieceOverlap1 = noPieceOverlap();
        bool noColorOverlap1 = noColorOverlap();
        bool piecesMatchArray1 = piecesMatchArray();
        bool playerColorsMatch1 = playerColorsMatch();
        if (!noPieceOverlap1)
            cout << "Piece overlap \n";
        if (!noColorOverlap1)
            cout << "Color overlap \n";
        if (!piecesMatchArray1)
            cout << "Pieces don't match array\n";
        if (!playerColorsMatch1) {
            cout << "Player colors don't match\n";
        }
        return noPieceOverlap1 && noColorOverlap1 && piecesMatchArray1 && playerColorsMatch1;
    }

    bool ChessBoard::noPieceOverlap() const {
        /// ensures that each Square is occupied by either exactly one piece, or by PIECE_NONE
        Bitboard pieces = BITBOARD_EMPTY;
        for (Piece piece = PIECE_FIRST; piece <= PIECE_LAST; ++piece) {
            if (pieces & bitboardOf(piece)) {
                return false;
            }
            pieces |= bitboardOf(piece);
        }
        return pieces == BITBOARD_FULL;
    }

    bool ChessBoard::noColorOverlap() const {
        /// ensures that each Square is occupied by either exactly one player, or by PIECE_NONE
        Bitboard pieces = bitboardOf(PIECE_NONE);
        if (pieces & bitboardOf(WHITE)) {
            return false;
        }
        pieces |= bitboardOf(WHITE);

        if (pieces & bitboardOf(BLACK)) {
            return false;
        }
        pieces |= bitboardOf(BLACK);

        return pieces == BITBOARD_FULL;
    }

    bool ChessBoard::playerColorsMatch() const {
        Bitboard white = BITBOARD_EMPTY;
        Bitboard black = BITBOARD_EMPTY;
        for (Piece piece = PIECE_FIRST_WHITE; piece <= PIECE_LAST_WHITE; ++piece) {
            white |= bitboardOf(piece);
        }

        for (Piece piece = PIECE_FIRST_BLACK; piece <= PIECE_LAST_BLACK; ++piece) {
            black |= bitboardOf(piece);
        }

        return (white == bitboardOf(WHITE)) && (black == bitboardOf(BLACK));
    }

    bool ChessBoard::piecesMatchArray() const {
        for (Square square = SQ_FIRST; square < SQ_LAST; ++square) {
            Piece actualPiece = getPieceOn(square);
            if (!(bitboardOf(actualPiece) & maskOf(square))) {
                return false;
            }
        }
        return true;
    }

    /// maybe also get rid of template<Player player>, since it just makes functions bigger, and harder to all keep in the cache

    template<Player player, MoveGenType moveGenType>
    void ChessBoard::generatePawnMoves(MoveChunk &moveChunk, Bitboard source, Bitboard target) const {
        // todo: pawn captures can never be losing, so make it an error if they are and don't add to losing move list.
        // todo: merge similar actions with captures for readability and speed
        // todo: enclose everything into scopes
        constexpr int direction = directionOf(player);
        constexpr int forward1shift = direction * 8;
        constexpr int forward2shift = 2 * forward1shift;

        constexpr int captureLeftShift = forward1shift - 1;
        constexpr int captureRightShift = forward1shift + 1;
        constexpr Bitboard shiftLeftMask = ~maskOf(FILE_A);
        constexpr Bitboard shiftRightMask = ~maskOf(FILE_H);

        constexpr Piece pawnPiece = makePiece(PIECE_TYPE_PAWN, player);
        constexpr Piece queen = makePiece(PIECE_TYPE_QUEEN, player);

        constexpr Player opponent = ~player;

        constexpr Rank rank2 = flipIfBlack(player, RANK_2);
        constexpr Bitboard rank2mask = maskOf(rank2);
        constexpr Rank rank7 = flipIfBlack(player, RANK_7);
        constexpr Bitboard rank7mask = maskOf(rank7);


        constexpr bool doCaptures = (moveGenType == MoveGenType::CAPTURES) || (moveGenType == MoveGenType::ALL);
        constexpr bool doNonCaptures = (moveGenType == MoveGenType::NON_CAPTURES) || (moveGenType == MoveGenType::ALL);
        constexpr bool doStaticEval =
                (moveGenType == MoveGenType::CAPTURES) || (moveGenType == MoveGenType::NON_CAPTURES);

        Bitboard pawns = bitboardOf(pawnPiece) & source;
        const Bitboard &emptySquares = bitboardOf(PIECE_NONE);
        Bitboard emptyTargetSquares = emptySquares & target;
        Bitboard enemyPieces = bitboardOf(~player) & target;
        Bitboard enPassantTarget = signedShift<forward1shift>(target);


        if constexpr (doCaptures) {
            {
                //promotion left
                Bitboard promotionLeft = signedShift<captureLeftShift>(pawns & rank7mask & shiftLeftMask) & enemyPieces;
                while (promotionLeft) {
                    Square dst = popLsb(&promotionLeft);
                    Square src = dst - captureLeftShift;
                    if constexpr (doStaticEval) {
                        moveChunk.moveList.addMove(
                                Move::promotionMove(src, dst, PIECE_TYPE_QUEEN, STATIC_SCORE_PROMOTION_CAPTURE)
                        );
                        moveChunk.losingCaptures.addNonQueenPromotions(src, dst);
                    } else {
                        moveChunk.moveList.addPromotions(src, dst);
                    }
                }
            }
            {
                //promotion right
                Bitboard promotionRight =
                        signedShift<captureRightShift>(pawns & rank7mask & shiftRightMask) & enemyPieces;
                while (promotionRight) {
                    Square dst = popLsb(&promotionRight);
                    Square src = dst - captureRightShift;

                    if constexpr (doStaticEval) {
                        moveChunk.moveList.addMove(
                                Move::promotionMove(src, dst, PIECE_TYPE_QUEEN, STATIC_SCORE_PROMOTION_CAPTURE)
                        );
                        moveChunk.losingCaptures.addNonQueenPromotions(src, dst);
                    } else {
                        moveChunk.moveList.addPromotions(src, dst);
                    }
                }
            }
            {
                // promotion forward
                Bitboard promotionForward = signedShift<forward1shift>(pawns & rank7mask) & emptyTargetSquares;
                while (promotionForward) {
                    Square dst = popLsb(&promotionForward);
                    Square src = dst - forward1shift;
                    if constexpr (doStaticEval) {
                        //todo: check if it's faster to just not calculate the static eval score and check directly using threatData
                        SquareMask dstMask = maskOf(dst);
                        if ((threatData->threatsOf(player) & dstMask) | !(threatData->threatsOf(opponent) & dstMask)) {
                            // the promotion is a winning promotion
                            moveChunk.moveList.addMove(
                                    Move::promotionMove(src, dst, PIECE_TYPE_QUEEN, STATIC_SCORE_PROMOTION)
                            );
                        } else {
                            // the promoted pawn will immediately be captured and the pawn will be lost
                            moveChunk.losingCaptures.addMove(
                                    Move::promotionMove(src, dst, PIECE_TYPE_QUEEN)
                            );
                        }
                        // all moves other than a promotion to a queen are added to losing captures, since it is very rare that they will be used
                        moveChunk.losingCaptures.addNonQueenPromotions(src, dst);

                    } else {
                        moveChunk.moveList.addPromotions(src, dst);
                    }
                }
            }

            {
                //capture left
                Bitboard captureLeft = signedShift<captureLeftShift>(pawns & ~rank7mask & shiftLeftMask) & enemyPieces;
                while (captureLeft) {
                    Square dst = popLsb(&captureLeft);
                    Square src = dst - captureLeftShift;
                    if constexpr (doStaticEval) {
                        StaticEvalScore staticScore = evalCapture(dst, pawnPiece);
                        if (staticScore >= 0) {
                            moveChunk.moveList.addMove(
                                    Move::normalMove(src, dst, staticScore)
                            );
                        } else {
                            moveChunk.losingCaptures.addMove(
                                    Move::normalMove(src, dst)
                            );
                        }
                    } else {
                        moveChunk.moveList.addMove(Move::normalMove(src, dst));
                    }
                }
            }

            {
                //capture right
                Bitboard captureRight =
                        signedShift<captureRightShift>(pawns & ~rank7mask & shiftRightMask) & enemyPieces;
                while (captureRight) {
                    Square dst = popLsb(&captureRight);
                    Square src = dst - captureRightShift;

                    if constexpr (doStaticEval) {
                        StaticEvalScore captureEval = evalCapture(dst, pawnPiece);
                        if (captureEval >= 0) {
                            moveChunk.moveList.addMove(
                                    Move::normalMove(src, dst, captureEval)
                            );
                        } else {
                            moveChunk.losingCaptures.addMove(
                                    Move::normalMove(src, dst)
                            );
                        }
                    } else {
                        moveChunk.moveList.addMove(
                                Move::normalMove(src, dst)
                        );
                    }

                }
            }
            if (enPassantSquare != SQ_INVALID) {
                {
                    // en passent capture left
                    Bitboard enPassantLeft =
                            signedShift<-1>(pawns & shiftLeftMask) & maskOf(enPassantSquare) & enPassantTarget;
                    if (enPassantLeft) {
                        Square src = enPassantSquare + 1;
                        Square dst = enPassantSquare + forward1shift;
                        if constexpr (doStaticEval) {
                            StaticEvalScore captureEval = evalCapture(dst, pawnPiece);
                            if (captureEval >= 0) {
                                moveChunk.moveList.addMove(
                                        Move::enPassantCapture(src, dst, captureEval)
                                );
                            } else {
                                moveChunk.losingCaptures.addMove(
                                        Move::enPassantCapture(src, dst)
                                );
                            }
                        } else {
                            moveChunk.moveList.addMove(
                                    Move::enPassantCapture(src, dst)
                            );
                        }
                    }
                }
                {
                    // en passent capture right
                    Bitboard enPassantRight =
                            signedShift<1>(pawns & shiftRightMask) & maskOf(enPassantSquare) & enPassantTarget;
                    if (enPassantRight) {
                        Square src = enPassantSquare - 1;
                        Square dst = enPassantSquare + forward1shift;
                        if constexpr (doStaticEval) {
                            StaticEvalScore captureEval = evalCapture(dst, pawnPiece);
                            if (captureEval >= 0) {
                                moveChunk.moveList.addMove(
                                        Move::enPassantCapture(src, dst, captureEval)
                                );
                            } else {
                                moveChunk.losingCaptures.addMove(
                                        Move::enPassantCapture(src, dst)
                                );
                            }
                        } else {
                            moveChunk.moveList.addMove(
                                    Move::enPassantCapture(src, dst)
                            );
                        }
                    }
                }
            }
        }


        if constexpr (doNonCaptures) {
            {
                // move forward 1
                Bitboard forward1Move = signedShift<forward1shift>(pawns & ~rank7mask) & emptyTargetSquares;
                while (forward1Move) {
                    Square dst = popLsb(&forward1Move);
                    moveChunk.moveList.addMove(
                            Move::normalMove(dst - forward1shift, dst)
                    );
                }
            }
            {
                //move forward 2
                Bitboard forward2Move =
                        signedShift<forward2shift>(pawns & rank2mask) & emptyTargetSquares &
                        signedShift<forward1shift>(emptySquares);
                while (forward2Move) {
                    Square dst = popLsb(&forward2Move);
                    moveChunk.moveList.addMove(
                            Move::pawnForward2(dst - forward2shift, dst)
                    );
                }
            }
        }
    }


    template<Player player, MoveGenType moveGenType>
    void ChessBoard::generateKnightMoves(MoveChunk &moveChunk, Bitboard source, Bitboard target) const {
        //it is assumed that if moveGenType is captures or non-capture, target is adjusted already
        constexpr Piece knightPiece = makePiece(PIECE_TYPE_KNIGHT, player);
        constexpr bool doStaticEval = moveGenType == MoveGenType::CAPTURES || moveGenType == MoveGenType::NON_CAPTURES;
        constexpr bool doCaptureEval = moveGenType == MoveGenType::CAPTURES;
        Bitboard knights = bitboardOf(knightPiece) & source;
        Bitboard availableSquares =
                ~bitboardOf(player) & target; //todo: this line repeats a lot. call bitwise and just once
        while (knights) {
            Square srcSquare = popLsb(&knights);
            Bitboard knightMoves = knightMovesFrom(srcSquare) & availableSquares;
            while (knightMoves) {
                Square dst = popLsb(&knightMoves);
                if constexpr (doCaptureEval) {
                    StaticEvalScore captureEval = evalCapture(dst, knightPiece);
                    if (captureEval >= 0) {
                        moveChunk.moveList.addMove(Move::normalMove(srcSquare, dst, captureEval));
                    } else {
                        moveChunk.losingCaptures.addMove(Move::normalMove(srcSquare, dst));
                    }
                } else {
                    moveChunk.moveList.addMove(Move::normalMove(srcSquare, dst));
                }
            }
        }
    }

    template<Player player, PieceType pieceType, MoveGenType moveGenType>
    void ChessBoard::generateSlidingPieceMoves(MoveChunk &moveChunk, Bitboard source, Bitboard target) const {
        constexpr Piece piece = makePiece(pieceType, player);
        constexpr Player opponent = ~player;

        constexpr bool doStaticEval = moveGenType == MoveGenType::CAPTURES || moveGenType == MoveGenType::NON_CAPTURES;
        constexpr bool doCaptureEval = moveGenType == MoveGenType::CAPTURES;

        Bitboard pieceBitboard = bitboardOf(piece) & source;
        Bitboard availableSquares = ~bitboardOf(player) & target;
        Bitboard otherPieces = bitboardOf(player) | bitboardOf(opponent);
        while (pieceBitboard) {
            Square srcSquare = popLsb(&pieceBitboard);
            Bitboard moves = slidingMovesFrom<pieceType>(srcSquare, otherPieces) &
                             availableSquares;//todo: is & availableSquares necessary
            while (moves) {
                Square dst = popLsb(&moves);
                if constexpr (doCaptureEval) {
                    StaticEvalScore captureEval = evalCapture(dst, piece);
                    if (captureEval >= 0) {
                        moveChunk.moveList.addMove(Move::normalMove(srcSquare, dst, captureEval));
                    } else {
                        moveChunk.losingCaptures.addMove(Move::normalMove(srcSquare, dst));
                    }
                } else {
                    moveChunk.moveList.addMove(Move::normalMove(srcSquare, dst));
                }
            }
        }
    }

    template<Player player, MoveGenType moveGenType>
    void ChessBoard::generateKingMoves(MoveChunk &moveChunk, Bitboard target) const {
        constexpr bool doStaticEval = moveGenType == CAPTURES || moveGenType == NON_CAPTURES;
        constexpr bool doCaptureEval = moveGenType == MoveGenType::CAPTURES;
        constexpr Piece kingPiece = makePiece(PIECE_TYPE_KING, player);
        constexpr Player opponent = ~player;
        Bitboard availableSquares = ~bitboardOf(player) & ~threatData->threatsOf(opponent) & target;
        Bitboard king = bitboardOf(kingPiece);
        assert(king);
        Square srcSquare = lsb(king);
        Bitboard kingMoves = kingMovesFrom(srcSquare) & availableSquares;
        while (kingMoves) {
            Square dst = popLsb(&kingMoves);
            if constexpr (doCaptureEval) {
                Piece dstPiece = getPieceOn(dst);
                StaticEvalScore captureScore = staticPieceValue(
                        dstPiece); // since legal king captures are always unprotected, there is no need for SEE
                moveChunk.moveList.addMove(Move::normalMove(srcSquare, dst, captureScore));
            } else {
                moveChunk.moveList.addMove(Move::normalMove(srcSquare, dst));
            }
        }
    }

    template<Player player, MoveGenType moveGenType>
    void ChessBoard::generateAllPieces(MoveChunk &moveChunk, Bitboard source, Bitboard target) const {
        //todo: add back pawn moves
        generateKnightMoves<player, moveGenType>(moveChunk, source, target);
        generateSlidingPieceMoves<player, PIECE_TYPE_BISHOP, moveGenType>(moveChunk, source, target);
        generateSlidingPieceMoves<player, PIECE_TYPE_ROOK, moveGenType>(moveChunk, source, target);
        generateSlidingPieceMoves<player, PIECE_TYPE_QUEEN, moveGenType>(moveChunk, source, target);
    }

    template<Player player, MoveGenType moveGenType>
    void ChessBoard::generateMovesForPlayer(MoveChunk &moveChunk) const {
        //todo: add const where necessary

        constexpr bool generateCaptures = moveGenType == CAPTURES || moveGenType == ALL;
        constexpr bool generateNonCaptures = moveGenType == NON_CAPTURES || moveGenType == ALL;
        const Bitboard target = targetForMoveGenType<player, moveGenType>();
        if (!threatData->isInDoubleCheck(player)) {
            if (generateNonCaptures && !threatData->isInCheck(player)) {
                //castling
                Bitboard pieces = bitboardOf(player) | bitboardOf(~player); //todo: speed up
                Bitboard threats = threatData->threatsOf(~player);

                if (mayCastleKingSide<player>() &&
                    CastlingData::kingSideCastleOf<player>().mayCastle(pieces, threats)) {
                    moveChunk.moveList.addMove(Move::castle(kingSideCastleOf<player>()));
                }

                if (mayCastleQueenSide<player>() &&
                    CastlingData::queenSideCastleOf<player>().mayCastle(pieces, threats)) {
                    moveChunk.moveList.addMove(Move::castle(queenSideCastleOf<player>()));
                }
            }
            Bitboard notPinned = ~threatData->kingBlockersOf(player);
            generatePawnMoves<player, moveGenType>(moveChunk, notPinned, threatData->checkEvasionSquares);
            generateAllPieces<player, moveGenType>(moveChunk, notPinned, target & threatData->checkEvasionSquares);

            Bitboard targetPinned = target & threatData->kingBlockersOf(player);

            if (!threatData->isInCheck(player) && targetPinned) {
                //calculate pins
                Piece kingPiece = makePiece(PIECE_TYPE_KING, player);
                Square kingSquare = lsb(bitboardOf(kingPiece));

                XrayData &rookXray = slidingPieceDataOf<PIECE_TYPE_ROOK>(kingSquare).xrayData;
                XrayData &bishopXray = slidingPieceDataOf<PIECE_TYPE_BISHOP>(kingSquare).xrayData;

                //todo: optimize for speed. Check whether ifs are helping or slowing down
                /// also think about not checking redundant pins, like a kingBlockers knight which can't bestMove_ at all
                /// like a rook kingBlockers on a diagonal or a bishop kingBlockers on a rank
                // todo: remove repeated bitwise &
                {
                    Bitboard kingRank = rookXray.rankFileDiagonal1();
                    if (targetPinned & kingRank) {
                        generatePawnMoves<player, moveGenType>(moveChunk, targetPinned & kingRank, kingRank);
                        generateAllPieces<player, moveGenType>(moveChunk, targetPinned & kingRank, kingRank);
                    }
                }
                {
                    Bitboard kingFile = rookXray.rankFileDiagonal2();
                    if (targetPinned & kingFile) {
                        generateAllPieces<player, moveGenType>(moveChunk, targetPinned & kingFile, kingFile);
                        generateAllPieces<player, moveGenType>(moveChunk, targetPinned & kingFile, kingFile);
                    }
                }

                {
                    Bitboard kingDiagonal1 = bishopXray.rankFileDiagonal1();
                    if (targetPinned & kingDiagonal1) {
                        generateAllPieces<player, moveGenType>(moveChunk, targetPinned & kingDiagonal1, kingDiagonal1);
                        generateAllPieces<player, moveGenType>(moveChunk, targetPinned & kingDiagonal1, kingDiagonal1);
                    }
                }

                {
                    Bitboard kingDiagonal2 = bishopXray.rankFileDiagonal2();
                    if (targetPinned & kingDiagonal2) {
                        generateAllPieces<player, moveGenType>(moveChunk, targetPinned & kingDiagonal2, kingDiagonal2);
                        generateAllPieces<player, moveGenType>(moveChunk, targetPinned & kingDiagonal2, kingDiagonal2);
                    }
                }
            }
        }
        generateKingMoves<player, moveGenType>(moveChunk, target);
    }

    template<MoveGenType moveGenType>
    void ChessBoard::generateMoves(MoveChunk &moveChunk) const {
        if (currentPlayer == WHITE) {
            generateMovesForPlayer<WHITE, moveGenType>(moveChunk);
        } else {
            generateMovesForPlayer<BLACK, moveGenType>(moveChunk);
        }
    }

    template void ChessBoard::generateMoves<ALL>(MoveChunk &moveChunk) const;

    template void ChessBoard::generateMoves<CAPTURES>(MoveChunk &moveChunk) const;

    template void ChessBoard::generateMoves<NON_CAPTURES>(MoveChunk &moveChunk) const;

    template<Player player>
    void ChessBoard::calculatePawnThreats() {
        constexpr int rankShift = directionOf(player);
        constexpr Piece pawnPiece = makePiece(PIECE_TYPE_PAWN, player);
        constexpr Piece enemyKing = makePiece(PIECE_TYPE_KING, ~player);
        constexpr Player opponent = ~player;
        //todo: check if bitboardOf should be extracted
        Bitboard &threats = threatData->threatsOf(pawnPiece);
        Bitboard &enemyKingBitboard = bitboardOf(enemyKing);
        threats = shiftWithMask<rankShift, 1>(bitboardOf(pawnPiece))
                  | shiftWithMask<rankShift, -1>(bitboardOf(pawnPiece));
        if (threats & enemyKingBitboard) {
            // the pawns have check
            if (threatData->isInCheck(opponent)) {
                threatData->isInDoubleCheck(opponent) = true;
            } else {
                threatData->isInCheck(opponent) = true;
                threatData->checkEvasionSquares = (shiftWithMask<-rankShift, 1>(enemyKingBitboard) |
                                                   shiftWithMask<-rankShift, -1>(enemyKingBitboard)) &
                                                  bitboardOf(pawnPiece);
                assert(populationCout(threatData->checkEvasionSquares) == 1);
            }
        }
    }


//todo: check if lookup or shift is better
    template<Player player>
    void ChessBoard::calculateKnightThreats() {
        constexpr Piece knightPiece = makePiece(PIECE_TYPE_KNIGHT, player);
        constexpr Piece enemyKing = makePiece(PIECE_TYPE_KING, ~player);
        constexpr Player opponent = ~player;
        Bitboard knights = bitboardOf(knightPiece);
        Bitboard &threats = threatData->threatsOf(knightPiece);
        while (knights) {
            Square square = popLsb(&knights);
            Bitboard moves = knightMovesFrom(square);
            threats |= moves;

            if (moves & bitboardOf(enemyKing)) {
                if (threatData->isInCheck(opponent)) {
                    threatData->isInDoubleCheck(opponent) = true;
                } else {
                    threatData->isInCheck(opponent) = true;
                    threatData->checkEvasionSquares = maskOf(square);
                }
            }
        }
    }

//todo: check if queen should be combined with bishop and rook
// (but remember that this may harm the value of pieces for static bestMove_ evaluation)
    template<Player player, PieceType pieceType>
    void ChessBoard::calculateSlidingPieceThreats() {
        constexpr Piece piece = makePiece(pieceType, player);
        constexpr Player opponent = ~player;
        constexpr Piece enemyKing = makePiece(PIECE_TYPE_KING, opponent);
        Bitboard pieceBitboard = bitboardOf(piece);
        Bitboard otherPieces = bitboardOf(player) | bitboardOf(opponent); //todo: speed up
        while (pieceBitboard) {
            Square square = popLsb(&pieceBitboard);
            SlidingPieceData &slidingPieceData = slidingPieceDataOf<pieceType>(square);
            Bitboard threats = slidingPieceData.magicHashData.calculateSlidingMoves(otherPieces);
            if (threats & bitboardOf(enemyKing)) {
                //check
                if (threatData->isInCheck(opponent)) {
                    threatData->isInDoubleCheck(opponent) = true;
                } else {
                    threatData->isInCheck(opponent) = true;
                    Bitboard checkDirection = slidingPieceData.xrayData.directionTo(bitboardOf(enemyKing));
                    if (checkDirection == BITBOARD_FULL) {
                        printBitboards();
                        cout << "Square: " << toString(square) << "\nthreatData: ";
                        printBitboard(threats);
                        cout << "otherPieces: ";
                        printBitboard(otherPieces);
                        cout << std::hex << "otherPiecesHex: 0x" << otherPieces << std::dec << "\n";
                        assert(false);
                    }
                    threatData->checkEvasionSquares = (checkDirection & threats) | maskOf(square);
                }
                threatData->threatsOf(piece) |= slidingPieceData.xrayData.allDirections;
            } else {
                threatData->threatsOf(piece) |= threats;

                //check pins
                if (slidingPieceData.xrayData.allDirections & bitboardOf(enemyKing)) {
                    //there are one or more pieces blocking the king. Check for exact pins.
                    Bitboard directionMask = slidingPieceData.xrayData.directionTo(bitboardOf(enemyKing));
                    Bitboard kingBlockersMask = slidingPieceData.magicHashData.calculateSlidingMoves
                            (bitboardOf(enemyKing)) & ~bitboardOf(enemyKing);
                    Bitboard blockers = directionMask & kingBlockersMask & otherPieces;
                    if (!blockers) {
                        printBitboards();
                        cout << "directionMask: \n";
                        printBitboard(directionMask);
                        cout << "kingBlockersMask: \n";
                        printBitboard(kingBlockersMask);
                        cout << "otherPieces: \n";
                        printBitboard(otherPieces);
                        assert(false);
                    }
                    Square firstBlockerSquare = popLsb(&blockers);
                    if (!blockers) {
                        // there is exactly one piece blocking the king, so it is a pin.
                        threatData->kingBlockersOf(opponent) |= maskOf(firstBlockerSquare);
                    }
                }
            }
        }
    }

    template<Player player>
    void ChessBoard::calculateQueenThreats() {
        //todo: merge with slidingPieceThreats
        constexpr Piece queenPiece = makePiece(PIECE_TYPE_QUEEN, player);
        constexpr Player opponent = ~player;
        constexpr Piece enemyKing = makePiece(PIECE_TYPE_KING, opponent);
        Bitboard queens = bitboardOf(queenPiece);
        Bitboard otherPieces = bitboardOf(player) | bitboardOf(opponent); //todo: speed up
        while (queens) {
            Square square = popLsb(&queens);
            SlidingPieceData &rookPieceData = slidingPieceDataOf<PIECE_TYPE_ROOK>(square);
            SlidingPieceData &bishopPieceData = slidingPieceDataOf<PIECE_TYPE_BISHOP>(square);
            Bitboard threats = rookPieceData.magicHashData.calculateSlidingMoves(otherPieces) |
                               bishopPieceData.magicHashData.calculateSlidingMoves(otherPieces);
            if (threats & bitboardOf(enemyKing)) {
                //check
                if (threatData->isInCheck(opponent)) {
                    threatData->isInDoubleCheck(opponent) = true;
                } else {
                    threatData->isInCheck(opponent) = true;
                    if (rookPieceData.xrayData.allDirections & bitboardOf(enemyKing)) {
                        Bitboard checkDirection = rookPieceData.xrayData.directionTo(bitboardOf(enemyKing));
                        threatData->checkEvasionSquares = (checkDirection & threats) | maskOf(square);
                    } else {
                        Bitboard checkDirection = bishopPieceData.xrayData.directionTo(bitboardOf(enemyKing));
                        threatData->checkEvasionSquares = (checkDirection & threats) | maskOf(square);
                    }
                }
                threatData->threatsOf(queenPiece) |= rookPieceData.xrayData.allDirections |
                                                     bishopPieceData.xrayData.allDirections;
            } else {
                threatData->threatsOf(queenPiece) |= threats;
                //check pins
                if ((rookPieceData.xrayData.allDirections | bishopPieceData.xrayData.allDirections)
                    & bitboardOf(enemyKing)) {
                    //there are one or more pieces blocking the king. Check for exact pins.
                    SlidingPieceData &slidingPieceData = (rookPieceData.xrayData.allDirections & bitboardOf(enemyKing))
                                                         ?
                                                         rookPieceData : bishopPieceData;
                    Bitboard directionMask = slidingPieceData.xrayData.directionTo(bitboardOf(enemyKing));
                    Bitboard kingBlockersMask = slidingPieceData.magicHashData.calculateSlidingMoves
                            (bitboardOf(enemyKing)) & ~bitboardOf(enemyKing);
                    Bitboard kingBlockers = directionMask & kingBlockersMask & otherPieces;
                    if (!kingBlockers) {
                        printBitboards();
                        cout << "directionMask: \n";
                        printBitboard(directionMask);
                        cout << "kingBlockersMask: \n";
                        printBitboard(kingBlockersMask);
                        cout << "otherPieces: \n";
                        printBitboard(otherPieces);
                        assert(false);
                    }
                    Square firstBlockerSquare = popLsb(&kingBlockers);
                    if (!kingBlockers) {
                        // there is exactly one piece blocking the king, so it is a pin.
                        threatData->kingBlockersOf(opponent) |= maskOf(firstBlockerSquare);
                    }
                }
            }

        }
    }

    template<Player player>
    void ChessBoard::calculateKingThreats() {
        constexpr Piece kingPiece = makePiece(PIECE_TYPE_KING, player);
        assert(kingPiece);
        Square kingSquare = lsb(bitboardOf(kingPiece));
        threatData->threatsOf(kingPiece) = kingMovesFrom(kingSquare);
    }

    template<Player player>
    inline void ChessBoard::calculateThreats() {
        //todo: initialize threatData beforehand
        calculatePawnThreats<player>();
        calculateKnightThreats<player>();
        calculateSlidingPieceThreats<player, PIECE_TYPE_BISHOP>();
        calculateSlidingPieceThreats<player, PIECE_TYPE_ROOK>();
        calculateQueenThreats<player>();
        calculateKingThreats<player>();

        // todo: optimize. maybe have each function return the threatData, instead of fetching them directly.
        /// See if that's faster. Or maybe use a for loop

        constexpr Piece pawnPiece = makePiece(PIECE_TYPE_PAWN, player);
        constexpr Piece knightPiece = makePiece(PIECE_TYPE_KNIGHT, player);
        constexpr Piece bishopPiece = makePiece(PIECE_TYPE_BISHOP, player);
        constexpr Piece rookPiece = makePiece(PIECE_TYPE_ROOK, player);
        constexpr Piece queenPiece = makePiece(PIECE_TYPE_QUEEN, player);
        constexpr Piece kingPiece = makePiece(PIECE_TYPE_KING, player);

        threatData->threatsOf(player) = threatData->threatsOf(pawnPiece) |
                                        threatData->threatsOf(knightPiece) |
                                        threatData->threatsOf(bishopPiece) |
                                        threatData->threatsOf(rookPiece) |
                                        threatData->threatsOf(queenPiece) |
                                        threatData->threatsOf(kingPiece);
    }

    Score ChessBoard::evaluateWhite() {
        Score scoreWhite = SCORE_ZERO;
        for (Piece piece = PIECE_FIRST_WHITE; piece <= PIECE_LAST_WHITE; ++piece) {
            scoreWhite += evalData.pieceScalar(piece) * bitboardOf(piece);
        }

        Score scoreBlack = SCORE_ZERO;
        for (Piece piece = PIECE_FIRST_BLACK; piece <= PIECE_LAST_BLACK; ++piece) {
            scoreBlack += evalData.pieceScalar(piece) * bitboardOf(piece);
        }
        return scoreWhite - scoreBlack;
    }

    ChessBoard &ChessBoard::operator=(const ChessBoard &other) {
        assert(&other != this);
        castlingRights = other.castlingRights;

        evalData = other.evalData;
        enPassantSquare = other.enPassantSquare;
        currentPlayer = other.currentPlayer;
        for (Piece piece = PIECE_FIRST; piece <= PIECE_LAST_NOT_EMPTY; ++piece) {
            pieceBitboards[piece] = other.pieceBitboards[piece];
        }

        pieceBitboards[PIECE_NONE] = other.pieceBitboards[PIECE_NONE];


        byPlayerBitboards[WHITE] = other.byPlayerBitboards[WHITE];
        byPlayerBitboards[BLACK] = other.byPlayerBitboards[BLACK];

        for (int square = SQ_FIRST; square <= SQ_LAST; ++square) {
            pieceBitboards[square] = other.pieceBitboards[square];
        }

        hashKey = other.hashKey;
        moveCount = other.moveCount;

        return *this;
    }

    bool ChessBoard::samePositionAs(const ChessBoard &other) const {

        if (!(castlingRights == other.castlingRights &&
              currentPlayer == other.currentPlayer &&
              enPassantSquare == other.enPassantSquare)) {
            return false;
        }

        for (Square square = SQ_FIRST; square <= SQ_LAST; ++square) {
            if (piecesBySquare[square] != other.piecesBySquare[square]) {
                cout << "Piece on Square unequal: " << toString(square) << "\n";
                return false;
            }
        }

        return true;
    }



    bool ChessBoard::parseFen(const string &fenString) {

        bool whiteMayCastleQueenSide = false;
        bool blackMayCastleQueenSide = false;
        bool whiteMayCastleKingSide = false;
        bool blackMayCastleKingSide = false;

        File file = FILE_FIRST;
        Rank rank = RANK_LAST;
        int i = 0;
        while (true) {
            if (i == fenString.size()) {
                return false;
            }
            char c = fenString[i];
            i++;
            switch (c) {
                case ' ':
                    break;
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8': {
                    int num = parseDigit(c);
                    for (int _ = 0; _ < num; _++) {
                        if (!file_ok(file)) {
                            return false;
                        }

                        pieceOn(makeSquare(rank, file)) = PIECE_NONE;
                        ++file;
                    }
                    break;
                }
                case 'P':
                case 'N':
                case 'B':
                case 'R':
                case 'Q':
                case 'K':
                case 'p':
                case 'n':
                case 'b':
                case 'r':
                case 'q':
                case 'k': {
                    if (!file_ok(file)) {
                        return false;
                    }
                    Piece piece = parsePiece(c);
                    pieceOn(makeSquare(rank, file)) = piece;
                    ++file;
                    break;
                }
                case '/': {
                    if (file != FILE_AFTER_LAST) {
                        return false;
                    }
                    --rank;
                    file = FILE_FIRST;
                    break;
                }
                default:
                    //invalid character
                    return false;
            }
            if (rank == RANK_FIRST && file == FILE_AFTER_LAST) {
                break;
            }
        }


        while (true) {
            if (i == fenString.size()) {
                return false;
            }
            char c = fenString[i];
            i++;
            switch (c) {
                case ' ':
                    break;
                case 'w':
                case 'W':
                    currentPlayer = WHITE;
                    goto endOfLoop;
                case 'b':
                case 'B':
                    currentPlayer = BLACK;
                    goto endOfLoop;
                default:
                    // illegal character
                    return false;
            }
        }

        endOfLoop:
        bool atLeastOneCastling = false;
        while (true) {
            if (i == fenString.size()) {
                return false;
            }
            char c = fenString[i];
            switch (c) {
                case ' ':
                    break;
                case 'K':
                    if (whiteMayCastleKingSide) {
                        // duplicate of the same castlingBits
                        return false;
                    }
                    whiteMayCastleKingSide = true;
                    atLeastOneCastling = true;
                    break;
                case 'Q':
                    if (whiteMayCastleQueenSide) {
                        // duplicate of the same castlingBits
                        return false;
                    }
                    whiteMayCastleQueenSide = true;
                    atLeastOneCastling = true;
                    break;
                case 'k':
                    if (blackMayCastleKingSide) {
                        // duplicate of the same castlingBits
                        return false;
                    }
                    blackMayCastleKingSide = true;
                    atLeastOneCastling = true;
                    break;
                case 'q':
                    if (blackMayCastleQueenSide) {
                        // duplicate of the same castlingBits
                        return false;
                    }
                    blackMayCastleQueenSide = true;
                    atLeastOneCastling = true;
                    break;
                case '-':
                    if (!atLeastOneCastling) {
                        i++;
                    }
                    goto endOfLoop2;

                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                    //does not increment i
                    goto endOfLoop2;

                default:
                    //illegal character
                    return false;
            }
            i++;
        }
        endOfLoop2:

        Rank enPassantRank = RANK_INVALID;
        File enPassantFile = FILE_INVALID;

        while (true) {
            if (i == fenString.size()) {
                return false;
            }
            char c = fenString[i];
            i++;
            switch (c) {
                case ' ':
                    break;
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                    if (!((enPassantFile == FILE_INVALID) && (enPassantRank == RANK_INVALID))) {
                        return false;
                    }
                    enPassantFile = parseFile(c);
                    break;
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                    if (!((enPassantFile != FILE_INVALID) && (enPassantRank == RANK_INVALID))) {
                        return false;
                    }
                    enPassantRank = parseRank(c);
                    enPassantSquare = makeSquare(enPassantRank, enPassantFile) - directionOf(currentPlayer) * 8;
                    goto endOfLoop3;
                case '-':
                    enPassantSquare = SQ_INVALID;
                    goto endOfLoop3;
                default:
                    return false;
            }
        }
        endOfLoop3:
        castlingRights = CASTLE_RIGHTS_NONE;
        if (whiteMayCastleKingSide) {
            castlingRights |= CASTLE_RIGHTS_WHITE_KING_SIDE;
        }
        if (whiteMayCastleQueenSide) {
            castlingRights |= CASTLE_RIGHTS_WHITE_QUEEN_SIDE;
        }
        if (blackMayCastleKingSide) {
            castlingRights |= CASTLE_RIGHTS_BLACK_KING_SIDE;
        }
        if (blackMayCastleQueenSide) {
            castlingRights |= CASTLE_RIGHTS_BLACK_QUEEN_SIDE;
        }
        updateBitboards();
        initHashKey();
        return true;
    }

    void ChessBoard::updateBitboards() {
        for (Piece piece = PIECE_FIRST; piece <= PIECE_LAST; ++piece) {
            bitboardOf(piece) = BITBOARD_EMPTY;
        }

        for (Square square = SQ_FIRST; square <= SQ_LAST; ++square) {
            Piece piece = pieceOn(square);
            bitboardOf(piece) |= maskOf(square);
        }
        updateBitboardsForPlayer(WHITE);
        updateBitboardsForPlayer(BLACK);

    }

    ostream &ChessBoard::getFen(ostream &outputStream) const {
        int numConnectedEmptySquares;
        for (Rank rank = RANK_LAST; rank >= RANK_FIRST; --rank) {
            numConnectedEmptySquares = 0;
            for (File file = FILE_FIRST; file <= FILE_LAST; ++file) {
                const Piece &piece = getPieceOn(makeSquare(rank, file));
                if (piece == PIECE_NONE) {
                    numConnectedEmptySquares++;
                } else {
                    if (numConnectedEmptySquares > 0) {
                        outputStream << numConnectedEmptySquares;
                        numConnectedEmptySquares = 0;
                    }

                    outputStream << toChar(piece);
                }
            }
            if (numConnectedEmptySquares > 0) {
                outputStream << numConnectedEmptySquares;
                numConnectedEmptySquares = 0;
            }
            if (rank != RANK_FIRST) {
                outputStream << "/";
            }
        }

        outputStream << " " << (currentPlayer == WHITE ? "w" : "b") << " ";

        if (mayCastleKingSide<WHITE>()) {
            outputStream << "K";
        }
        if (mayCastleQueenSide<WHITE>()) {
            outputStream << "Q";
        }
        if (mayCastleKingSide<BLACK>()) {
            outputStream << "k";
        }
        if (mayCastleQueenSide<BLACK>()) {
            outputStream << "q";
        }

        if (castlingRights == CASTLE_RIGHTS_NONE) {
            outputStream << "-";
        }
        outputStream << " ";

        outputStream
                << (enPassantSquare == SQ_INVALID ? "-" : toString(enPassantSquare + directionOf(currentPlayer) * 8));

        outputStream << " 0 1";

        return outputStream;
    }

    void ChessBoard::updateBitboardsForPlayer(Player player) {
        bitboardOf(player) = BITBOARD_EMPTY;
        for (Piece piece = firstPieceOf(player); piece <= lastPieceOf(player); ++piece) {
            bitboardOf(player) |= bitboardOf(piece);
        }
    }

    bool ChessBoard::doMoves(vector<MoveInputData> &moves) {
        for (MoveInputData &moveInputData:moves) {
            if (!moveInputData.isOk()) {
                return false;
            }
            MoveChunk moveChunk;
            generateThreatsAndMoves(moveChunk);

            Move move = moveChunk.moveList.getMoveFromInputData(moveInputData);
            if (move.isOk()) {
                doMove(move);
            } else {
                return false;
            }
        }
        return true;
    }

    bool ChessBoard::doMoves(stringstream &moveStream) {
        for (string str; moveStream >> str && !str.empty();) {
            MoveInputData moveInputData = MoveInputData::parse(str);
            if (!moveInputData.isOk()) {
                return false;
            }
            MoveChunk moveChunk;
            generateThreatsAndMoves(moveChunk);
            Move move = moveChunk.moveList.getMoveFromInputData(moveInputData);
            if (move.isOk()) {
                doGameMove(move);
            } else {
                return false;
            }
        }
        return true;
    }

    void ChessBoard::initHashKey() {
        hashKey = KEY_ZERO;
        for (Square square = SQ_FIRST; square <= SQ_LAST; ++square) {
            Piece piece = pieceOn(square);
            if (piece != PIECE_NONE) {
                hashKey ^= zobristData.keyOf(square, piece);
            }
        }
        if (currentPlayer == BLACK) {
            hashKey ^= zobristData.keyBlackToMove();
        }

        if (enPassantSquare != SQ_INVALID) {
            hashKey ^= zobristData.keyOf(fileOf(enPassantSquare));
        }

        hashKey ^= zobristData.keyOf(castlingRights);
    }

    MoveRevertData ChessBoard::doGameMove(Move move) {
        moveCount++;
        if (isIreversible(move)) {
            positionsForRepetition.clear();
        }
        MoveRevertData moveRevertData = doMove(move);
        positionsForRepetition.push_back(getHashKey());
        return moveRevertData;
    }

    set<Key> ChessBoard::getRepeatedPositions() {
        set<Key> repeatedPositions{};
        for (int i = 0; i < positionsForRepetition.size(); ++i) {
            const Key &key = positionsForRepetition[i];
            // if the key is already in the set, we continue, since we don't want the same key twice
            if (repeatedPositions.count(key)) {
                continue;
            }
            for (int j = i + 1; j < positionsForRepetition.size(); ++j) {
                if (positionsForRepetition[j] == key) {
                    repeatedPositions.insert(key);
                    break;
                }
            }
        }
        return repeatedPositions;
    }

    void ChessBoard::calculateAllThreats() {
        assert(threatData->isEmpty());
        // always calculate threatData for the inactive player first
        if (currentPlayer == WHITE) {
            calculateThreats<BLACK>();
            calculateThreats<WHITE>();
        } else {
            calculateThreats<WHITE>();
            calculateThreats<BLACK>();
        }
    }

    /// todo: see if this should be inline or not
    /// see if loop unrolling can help. See if an incrementally updated makeThreatMap mask can help. See if there's a way to optimize multiple pieces attacking the same square.
    ThreatMap ThreatData::makeThreatMap(SquareMask squareMask) const {
        assert(this != nullptr);
        ThreatMap threatMap = THREAT_MAP_NONE;
        for (Piece piece = PIECE_FIRST; piece <= PIECE_LAST_NOT_EMPTY; ++piece) {
            if (threatsOf(piece) & squareMask) {
                threatMap |= threatMapOf(piece);
            }
        }
        return threatMap;
    }

    std::ostream &operator<<(ostream &os, ThreatData &threatData) {
        for (Piece piece=PIECE_FIRST; piece<=PIECE_LAST_NOT_EMPTY;++piece){
            cout << "threats of " << piece << ":\n";
            printBitboard(threatData.threatsOf(piece), cout);
        }

        cout << "threats of WHITE: \n";
        printBitboard(threatData.threatsOf(WHITE));

        cout << "threats of BLACK: \n";
        printBitboard(threatData.threatsOf(BLACK));
        return os;
    }
}