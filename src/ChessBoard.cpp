//
// Created by guykn on 12/7/2020.
//

#include <string>
#include <jsInterface.h>
#include "ChessBoard.h"
#include "iostream"
#include "Bitboards.h"

namespace Chess {

    //todo: implement game history
    //todo: investigate occasional weird bugs (bishop turning into rook)

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
        isCheck = false;
        isDoubleCheck = false;

        whiteMayCastleKingSide = true;
        whiteMayCastleQueenSide = true;
        blackMayCastleKingSide = true;
        blackMayCastleQueenSide = true;

        checkEvasionSquares = BITBOARD_FULL;
        pinned = BITBOARD_EMPTY;


        for (Piece piece = PIECE_FIRST; piece <= PIECE_LAST_NOT_EMPTY; ++piece) {
            bitboardOf(piece) = BITBOARD_EMPTY;
            threatsOf(piece) = BITBOARD_EMPTY;
        }

        bitboardOf(PIECE_NONE) = BITBOARD_EMPTY;

        bitboardOf(WHITE) = BITBOARD_EMPTY;
        bitboardOf(BLACK) = BITBOARD_EMPTY;

        threatsOf(WHITE) = BITBOARD_EMPTY;
        threatsOf(BLACK) = BITBOARD_EMPTY;

        SquareMask squareMask = SQUARE_MASK_FIRST;
        for (Square square = SQ_FIRST; square <= SQ_LAST; ++square) {
            Piece piece = piecesArray[square];
            pieceOn(square) = piece;
            bitboardOf(piece) |= squareMask;
            if (piece != PIECE_NONE) {
                bitboardOf(playerOf(piece)) |= squareMask;
            }
            squareMask <<= 1;
        }
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
        cout << "\nCurrent player: " << currentPlayer;

        cout << "\n" << (isCheck ? "Check" : "Not Check");

        cout << "\n" << (isDoubleCheck ? "Double Check" : "Not Double Check");

        cout << "\nPinned: ";

        printBitboard(pinned);

        cout << "\nCheckEvasionsSquares: ";

        printBitboard(checkEvasionSquares);

        cout << "\nWhite Pawns:";
        printBitboard(getBitboardOf(PIECE_WHITE_PAWN));

        cout << "\nWhite Knights";
        printBitboard(getBitboardOf(PIECE_WHITE_KNIGHT));

        cout << "\nWhite Bishops";
        printBitboard(getBitboardOf(PIECE_WHITE_BISHOP));

        cout << "\nWhite Rook";
        printBitboard(getBitboardOf(PIECE_WHITE_ROOK));

        cout << "\nWhite Queen";
        printBitboard(getBitboardOf(PIECE_WHITE_QUEEN));

        cout << "\nWhite King";
        printBitboard(getBitboardOf(PIECE_WHITE_KING));

        cout << "\nBlack Pawns:";
        printBitboard(getBitboardOf(PIECE_BLACK_PAWN));

        cout << "\nBlack Knights";
        printBitboard(getBitboardOf(PIECE_BLACK_KNIGHT));

        cout << "\nBlack Bishops";
        printBitboard(getBitboardOf(PIECE_BLACK_BISHOP));

        cout << "\nBlack Rook";
        printBitboard(getBitboardOf(PIECE_BLACK_ROOK));

        cout << "\nBlack Queen";
        printBitboard(getBitboardOf(PIECE_BLACK_QUEEN));

        cout << "\nBlack King";
        printBitboard(getBitboardOf(PIECE_BLACK_KING));

        cout << "\nEmpty squares:";
        printBitboard(getBitboardOf(PIECE_NONE));

        cout << "\nWhite Pieces:";
        printBitboard(getBitboardOf(WHITE));

        cout << "\nBlack Pieces:";
        printBitboard(getBitboardOf(BLACK));

        cout << *this;
    }

    //todo: make this not run in release mode
    void ChessBoard::assertOk() const {
        if (!isOk()) {
            printBitboards();
            assert(false);
        }
    }

    void ChessBoard::doMoveOld(Move &move) {
        /// returns the WinState after the move

        //todo: see if execution time changes between making masks once as variable or as temp objects
        assert(move.isOk());
        assert(move.dstPiece == pieceOn(move.dstSquare));

        bitboardOf(PIECE_NONE) |= maskOf(move.srcSquare);
        (bitboardOf(move.srcPiece) &= notSquareMask(move.srcSquare));
        (bitboardOf(move.dstPiece)) &= notSquareMask(move.dstSquare);
        bitboardOf(move.promotionPiece) |= maskOf(move.dstSquare);
        ((bitboardOf(currentPlayer)) &= notSquareMask(move.srcSquare)) |= maskOf(move.dstSquare);
        bitboardOf(~currentPlayer) &= notSquareMask(move.dstSquare);

        pieceOn(move.srcSquare) = PIECE_NONE;
        pieceOn(move.dstSquare) = move.promotionPiece;

        if (move.isEnPassant) {
            Piece enemyPawn = makePiece(PIECE_TYPE_PAWN, ~currentPlayer);
            bitboardOf(PIECE_NONE) |= maskOf(enPassantSquare);
            bitboardOf(enemyPawn) &= notSquareMask(enPassantSquare);
            bitboardOf(~currentPlayer) &= notSquareMask(enPassantSquare);
            pieceOn(enPassantSquare) = PIECE_NONE;
        } else if (move.castlingType != CASTLE_NONE) {
            Piece rook = makePiece(PIECE_TYPE_ROOK, currentPlayer);
            CastlingData castlingData = CastlingData::fromCastlingType(move.castlingType);

            (bitboardOf(rook) &= notSquareMask(castlingData.rookSrc)) |= maskOf(castlingData.rookDst);
            ((bitboardOf(PIECE_NONE)) &= notSquareMask(castlingData.rookDst)) |= maskOf(castlingData.rookSrc);

            ((bitboardOf(currentPlayer)) &= notSquareMask(castlingData.rookSrc)) |= maskOf(castlingData.rookDst);

            pieceOn(castlingData.rookSrc) = PIECE_NONE;
            pieceOn(castlingData.rookDst) = rook;
        }

        enPassantSquare = move.pawnForward2Square;

        // update castling legality
        updateCastling(move);
        swapPlayer();
    }

    MoveRevertData ChessBoard::doMove(Move &move) {
        assert(move.isOk()); //inside of Chessboard.cpp
        assert(move.dstPiece == pieceOn(move.dstSquare));
        removePieceFrom(move.srcSquare, move.srcPiece, currentPlayer);
        setPieceOn(move.dstSquare, move.dstPiece, move.srcPiece, currentPlayer);

        if (move.castlingType != CASTLE_NONE) {
            Piece rook = makePiece(PIECE_TYPE_ROOK, currentPlayer);
            CastlingData castlingData = CastlingData::fromCastlingType(move.castlingType);
            removePieceFrom(castlingData.rookSrc, rook, currentPlayer);
            placePieceOn(castlingData.rookDst, rook, currentPlayer);
        } else if (move.isEnPassant) {
            Piece enemyPawn = makePiece(PIECE_TYPE_PAWN, ~currentPlayer);
            removePieceFrom(enPassantSquare, enemyPawn, ~currentPlayer);
        }
        enPassantSquare = move.pawnForward2Square;
        MoveRevertData moveRevertData = getMoveRevertData();
        updateCastling(move);
        swapPlayer();
        return moveRevertData;
    }

    void ChessBoard::undoMove(Move &move, MoveRevertData &moveRevertData) {
        revertTo(moveRevertData);
        //todo: incorperate promotions
        Player opponent = ~currentPlayer;
        placePieceOn(move.srcSquare, move.srcPiece, opponent);
        setPieceOn2(move.dstSquare, move.srcPiece, opponent, move.dstPiece);
        if (move.isEnPassant) {
            Piece pawn = makePiece(PIECE_TYPE_PAWN, currentPlayer);
            Square prevEnPassantSquare = move.dstSquare + 8 * directionOf(currentPlayer);
            placePieceOn(prevEnPassantSquare, pawn, currentPlayer);
        } else if (move.castlingType != CASTLE_NONE){
            Piece rook = makePiece(PIECE_TYPE_ROOK, opponent);
            CastlingData castlingData = CastlingData::fromCastlingType(move.castlingType);
            removePieceFrom(castlingData.rookDst, rook, opponent);
            placePieceOn(castlingData.rookSrc, rook, opponent);
        }
        swapPlayer();
    }

    void ChessBoard::revertTo(const MoveRevertData &moveRevertData) {
        whiteMayCastleKingSide = moveRevertData.whiteMayCastleKingSide;
        whiteMayCastleQueenSide = moveRevertData.whiteMayCastleQueenSide;
        blackMayCastleKingSide = moveRevertData.blackMayCastleKingSide;
        blackMayCastleQueenSide = moveRevertData.blackMayCastleQueenSide;
    }

    //todo: should this be inline?
    void ChessBoard::updateCastling(Move &move) {
        Bitboard moveSquares = maskOf(move.srcSquare) | maskOf(move.dstSquare);
        whiteMayCastleKingSide = whiteMayCastleKingSide &&
                                 !CastlingData::fromCastlingType(CASTLE_WHITE_KING_SIDE)
                                         .moveDisablesCastling(moveSquares);

        whiteMayCastleQueenSide = whiteMayCastleQueenSide &&
                                  !CastlingData::fromCastlingType(CASTLE_WHITE_QUEEN_SIDE)
                                          .moveDisablesCastling(moveSquares);

        blackMayCastleKingSide = blackMayCastleKingSide &&
                                 !CastlingData::fromCastlingType(CASTLE_BLACK_KING_SIDE)
                                         .moveDisablesCastling(moveSquares);

        blackMayCastleQueenSide = blackMayCastleQueenSide &&
                                  !CastlingData::fromCastlingType(CASTLE_BLACK_QUEEN_SIDE)
                                          .moveDisablesCastling(moveSquares);
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
        if(!piecesMatchArray1)
            cout << "Pieces don't match array\n";
        if(!playerColorsMatch1){
            cout << "Player colors don't match\n";
        }
        return noPieceOverlap1 && noColorOverlap1 && piecesMatchArray1 && playerColorsMatch1;
    }

    bool ChessBoard::noPieceOverlap() const {
        /// ensures that each Square is occupied by either exactly one piece, or by PIECE_NONE
        Bitboard pieces = BITBOARD_EMPTY;
        for (Piece piece = PIECE_FIRST; piece <= PIECE_LAST; ++piece) {
            if (pieces & getBitboardOf(piece)) {
                return false;
            }
            pieces |= getBitboardOf(piece);
        }
        return pieces == BITBOARD_FULL;
    }

    bool ChessBoard::noColorOverlap() const {
        /// ensures that each Square is occupied by either exactly one player, or by PIECE_NONE
        Bitboard pieces = getBitboardOf(PIECE_NONE);
        if (pieces & getBitboardOf(WHITE)) {
            return false;
        }
        pieces |= getBitboardOf(WHITE);

        if (pieces & getBitboardOf(BLACK)) {
            return false;
        }
        pieces |= getBitboardOf(BLACK);

        return pieces == BITBOARD_FULL;
    }

    bool ChessBoard::playerColorsMatch() const{
        Bitboard white = BITBOARD_EMPTY;
        Bitboard black = BITBOARD_EMPTY;
        for (Piece piece = PIECE_FIRST_WHITE; piece<=PIECE_LAST_WHITE;++piece){
            white|=getBitboardOf(piece);
        }

        for (Piece piece = PIECE_FIRST_BLACK; piece<=PIECE_LAST_BLACK;++piece){
            black|=getBitboardOf(piece);
        }

        return (white == getBitboardOf(WHITE)) && (black == getBitboardOf(BLACK));
    }

    bool ChessBoard::piecesMatchArray() const{
        for (Square square = SQ_FIRST; square<SQ_LAST; ++square){
            Piece actualPiece = getPieceOn(square);
            if(!(getBitboardOf(actualPiece) & maskOf(square))){
                return false;
            }
        }
        return true;
    }

    /// maybe also get rid of template<Player player>, since it just makes functions bigger, and harder to all keep in the cache

    template<Player player>
    void ChessBoard::generatePawnMoves(MoveList &moveList, Bitboard source, Bitboard target) {
        // note: promotionPiece is the piece that the pawns promote to, but it may also be PIECE_PAWN, which means that the pawns don't promote
        // only make promotionPiece into queen when it is know the pawns will promote this move
        constexpr int direction = directionOf(player);
        constexpr int forward1shift = direction * 8;
        constexpr int forward2shift = 2 * forward1shift;

        constexpr int captureLeftShift = forward1shift - 1;
        constexpr int captureRightShift = forward1shift + 1;
        constexpr Bitboard shiftLeftMask = ~maskOf(FILE_A);
        constexpr Bitboard shiftRightMask = ~maskOf(FILE_H);

        constexpr Piece pawnPiece = makePiece(PIECE_TYPE_PAWN, player);
        constexpr Piece queen = makePiece(PIECE_TYPE_QUEEN, player);

        constexpr Rank rank2 = flipIfBlack(player, RANK_2);
        constexpr Bitboard rank2mask = maskOf(rank2);
        constexpr Rank rank7 = flipIfBlack(player, RANK_7);
        constexpr Bitboard rank7mask = maskOf(rank7);

        Bitboard pawns = bitboardOf(pawnPiece) & source;
        Bitboard emptySquares = bitboardOf(PIECE_NONE) & target;
        Bitboard enemyPieces = bitboardOf(~player) & target;


        // move forward 1
        Bitboard forward1Move = signedShift<forward1shift>(pawns & ~rank7mask) & emptySquares;
        while (forward1Move) {
            Square dst = popLsb(&forward1Move);
            moveList.addMove(Move(dst - forward1shift, dst, pawnPiece, pieceOn(dst)));
        }

        //move forward 2
        Bitboard forward2Move = signedShift<forward2shift>(pawns & rank2mask ) & emptySquares & signedShift<forward1shift>(emptySquares);
        while (forward2Move) {
            Square dst = popLsb(&forward2Move);
            moveList.addMove(
                    Move::pawnForward2(dst - forward2shift, dst, pawnPiece, pieceOn(dst))
            );
        }

        //capture left
        Bitboard captureLeft = signedShift<captureLeftShift>(pawns & ~rank7mask & shiftLeftMask) & enemyPieces;
        while (captureLeft) {
            Square dst = popLsb(&captureLeft);
            moveList.addMove(Move(dst - captureLeftShift, dst, pawnPiece, pieceOn(dst)));
        }

        //capture right
        Bitboard captureRight = signedShift<captureRightShift>(pawns & ~rank7mask & shiftRightMask) & enemyPieces;
        while (captureRight) {
            Square dst = popLsb(&captureRight);
            moveList.addMove(Move(dst - captureRightShift, dst, pawnPiece, pieceOn(dst)));
        }

        if constexpr (!DISABLE_SPECIAL_MOVES) {
            if (enPassantSquare != SQ_INVALID) {
                // en passent capture left
                Bitboard enPassantLeft = signedShift<-1>(pawns & shiftLeftMask) & maskOf(enPassantSquare) & target;
                if (enPassantLeft) {
                    Square dst = enPassantSquare + forward1shift;
                    moveList.addMove(Move::enPassant(enPassantSquare + 1, dst, pawnPiece, pieceOn(dst)));
                }

                // en passent capture right
                Bitboard enPassantRight = signedShift<1>(pawns & shiftRightMask) & maskOf(enPassantSquare) & target;
                if (enPassantRight) {
                    Square dst = enPassantSquare + forward1shift;
                    moveList.addMove(Move::enPassant(enPassantSquare - 1, dst, pawnPiece, pieceOn(dst)));
                }
            }
        }


        if constexpr (!DISABLE_SPECIAL_MOVES) {
            // promotion forward
            Bitboard promotionForward = signedShift<forward1shift>(pawns & rank7mask) & emptySquares;
            while (promotionForward) {
                Square dst = popLsb(&promotionForward);
                moveList.addMove(
                        Move::promotion(dst - forward1shift, dst, pawnPiece, pieceOn(dst), queen)
                );
            }

            //promotion left
            Bitboard promotionLeft = signedShift<captureLeftShift>(pawns & rank7mask & shiftLeftMask) & enemyPieces;
            while (promotionLeft) {
                Square dst = popLsb(&promotionLeft);
                moveList.addMove(
                        Move::promotion(dst - captureLeftShift, dst, pawnPiece, pieceOn(dst), queen)
                );
            }

            //promotion right
            Bitboard promotionRight = signedShift<captureRightShift>(pawns & rank7mask & shiftRightMask) & enemyPieces;
            while (promotionRight) {
                Square dst = popLsb(&promotionRight);
                moveList.addMove(
                        Move::promotion(dst - captureRightShift, dst, pawnPiece, pieceOn(dst), queen)
                );
            }
        }
    }


    template<Player player>
    void ChessBoard::generateKnightMoves(MoveList &moveList, Bitboard source, Bitboard target) {
        constexpr Piece knightPiece = makePiece(PIECE_TYPE_KNIGHT, player);
        Bitboard knights = getBitboardOf(knightPiece) & source;
        Bitboard availableSquares = ~bitboardOf(player) & target;
        while (knights) {
            Square srcSquare = popLsb(&knights);
            Bitboard knightMoves = knightMovesFrom(srcSquare) & availableSquares;
            while (knightMoves) {
                Square dstSquare = popLsb(&knightMoves);
                moveList.addMove(Move(srcSquare, dstSquare, knightPiece, pieceOn(dstSquare)));
            }
        }
    }

    template<Player player, PieceType pieceType>
    void ChessBoard::generateSlidingPieceMoves(MoveList &moveList, Bitboard source, Bitboard target) {
        constexpr Piece piece = makePiece(pieceType, player);
        constexpr Player opponent = ~player;
        Bitboard pieceBitboard = getBitboardOf(piece) & source;
        Bitboard availableSquares = ~bitboardOf(player) & target;
        Bitboard otherPieces = bitboardOf(player) | bitboardOf(opponent);
        while (pieceBitboard) {
            Square srcSquare = popLsb(&pieceBitboard);
            Bitboard moves = slidingMovesFrom<pieceType>(srcSquare, otherPieces) &
                             availableSquares;//todo: is | availableSquares necessary
            while (moves) {
                Square dstSquare = popLsb(&moves);
                moveList.addMove(Move(srcSquare, dstSquare, piece, pieceOn(dstSquare)));
            }
        }
    }

    template<Player player>
    void ChessBoard::generateKingMoves(MoveList &moveList) {
        constexpr Piece kingPiece = makePiece(PIECE_TYPE_KING, player);
        constexpr Player opponent = ~player;
        Bitboard availableSquares = ~bitboardOf(player) & ~threatsOf(opponent);
        Bitboard king = bitboardOf(kingPiece);
        assert(king);
        Square srcSquare = lsb(king);
        Bitboard kingMoves = kingMovesFrom(srcSquare) & availableSquares;
        while (kingMoves) {
            Square dstSquare = popLsb(&kingMoves);
            moveList.addMove(Move(srcSquare, dstSquare, kingPiece, pieceOn(dstSquare)));
        }
    }

    template<Player player>
    void ChessBoard::generateAllPieces(MoveList &moveList, Bitboard source, Bitboard target) {
        generatePawnMoves<player>(moveList, source, target);
        generateKnightMoves<player>(moveList, source, target);
        generateSlidingPieceMoves<player, PIECE_TYPE_BISHOP>(moveList, source, target);
        generateSlidingPieceMoves<player, PIECE_TYPE_ROOK>(moveList, source, target);
        generateSlidingPieceMoves<player, PIECE_TYPE_QUEEN>(moveList, source, target);
    }

    template<Player player>
    GameEndState ChessBoard::generateMovesForPlayer(MoveList &moveList) {
        calculateThreats<~player>();
        generateKingMoves<player>(moveList);
        if (!isDoubleCheck) {
            Bitboard notPinned = ~pinned;
            generateAllPieces<player>(moveList, notPinned, checkEvasionSquares);
            if (!isCheck) {
                if constexpr (!DISABLE_SPECIAL_MOVES) {
                    Bitboard pieces = bitboardOf(player) | bitboardOf(~player); //todo: speed up
                    Bitboard threats = threatsOf(~player);

                    if (mayCastleKingSide<player>() &&
                        CastlingData::kingSideCastleOf<player>().mayCastle(pieces, threats)) {
                        moveList.addMove(Move::fromCastlingType(kingSideCastleOf<player>()));
                    }

                    if (mayCastleQueenSide<player>() &&
                        CastlingData::queenSideCastleOf<player>().mayCastle(pieces, threats)) {
                        moveList.addMove(Move::fromCastlingType(queenSideCastleOf<player>()));
                    }
                }


                if (pinned) {
                    if constexpr (!DISABLE_SPECIAL_MOVES || true) {
                        //calculate pins
                        Piece kingPiece = makePiece(PIECE_TYPE_KING, player);
                        Square kingSquare = lsb(bitboardOf(kingPiece));

                        XrayData rookXray = slidingPieceDataOf<PIECE_TYPE_ROOK>(kingSquare).xrayData;
                        XrayData bishopXray = slidingPieceDataOf<PIECE_TYPE_BISHOP>(kingSquare).xrayData;

                        Bitboard kingRank = rookXray.rankFileDiagonal1();
                        Bitboard kingFile = rookXray.rankFileDiagonal2();
                        Bitboard kingDiagonal1 = bishopXray.rankFileDiagonal1();
                        Bitboard kingDiagonal2 = bishopXray.rankFileDiagonal2();

                        //todo: optimize for speed. Check whether ifs are helping or slowing down
                        /// also think about not checking redundant pins, like a pinned knight which can't move at all
                        /// like a rook pinned on a diagonal or a bishop pinned on a rank

                        if (pinned & kingRank) {
                            generateAllPieces<player>(moveList, pinned & kingRank, kingRank);
                        }

                        if (pinned & kingFile) {
                            generateAllPieces<player>(moveList, pinned & kingFile, kingFile);
                        }

                        if (pinned & kingDiagonal1) {
                            generateAllPieces<player>(moveList, pinned & kingDiagonal1, kingDiagonal1);
                        }

                        if (pinned & kingDiagonal2) {
                            generateAllPieces<player>(moveList, pinned & kingDiagonal2, kingDiagonal2);
                        }
                    }
                }
            }
        }

        if (!moveList.isEmpty()) {
            return NO_GAME_END;
        } else if (isCheck) {
            return MATED;
        } else {
            return DRAW;
        }
    }

    GameEndState ChessBoard::generateMoves(MoveList &moveList) {
        if (currentPlayer == WHITE) {
            return generateMovesForPlayer<WHITE>(moveList);
        } else {
            return generateMovesForPlayer<BLACK>(moveList);
        }
    }

    template<Player player>
    void ChessBoard::calculatePawnThreats() {
        constexpr int rankShift = directionOf(player);
        constexpr Piece pawnPiece = makePiece(PIECE_TYPE_PAWN, player);
        constexpr Piece enemyKing = makePiece(PIECE_TYPE_KING, ~player);
        //todo: check if bitboardOf should be extracted
        Bitboard &threats = threatsOf(pawnPiece);
        Bitboard &enemyKingBitboard = bitboardOf(enemyKing);
        threats = shiftWithMask<rankShift, 1>(bitboardOf(pawnPiece))
                  | shiftWithMask<rankShift, -1>(bitboardOf(pawnPiece));
        if (threats & enemyKingBitboard) {
            // the pawns have check
            if (isCheck) {
                isDoubleCheck = true;
            } else {
                isCheck = true;
                checkEvasionSquares = (shiftWithMask<-rankShift, 1>(enemyKingBitboard) |
                                       shiftWithMask<-rankShift, -1>(enemyKingBitboard)) & bitboardOf(pawnPiece);
                assert(populationCout(checkEvasionSquares) == 1);
            }
        }
    }


//todo: check if lookup or shift is better
    template<Player player>
    void ChessBoard::calculateKnightThreats() {
        constexpr Piece knightPiece = makePiece(PIECE_TYPE_KNIGHT, player);
        constexpr Piece enemyKing = makePiece(PIECE_TYPE_KING, ~player);
        Bitboard knights = bitboardOf(knightPiece);
        threatsOf(knightPiece) = BITBOARD_EMPTY;
        while (knights) {
            Square square = popLsb(&knights);
            Bitboard moves = knightMovesFrom(square);
            threatsOf(knightPiece) |= moves;

            if (moves & bitboardOf(enemyKing)) {
                if (isCheck) {
                    isDoubleCheck = true;
                } else {
                    isCheck = true;
                    checkEvasionSquares = maskOf(square);
                }
            }
        }
    }

//todo: check if queen should be combined with bishop and rook
// (but remember that this may harm the value of pieces for static move evaluation)
    template<Player player, PieceType pieceType>
    void ChessBoard::calculateSlidingPieceThreats() {
        constexpr Piece piece = makePiece(pieceType, player);
        constexpr Player opponent = ~player;
        constexpr Piece enemyKing = makePiece(PIECE_TYPE_KING, opponent);
        Bitboard pieceBitboard = bitboardOf(piece);
        Bitboard otherPieces = bitboardOf(player) | bitboardOf(opponent); //todo: speed up
        threatsOf(piece) = BITBOARD_EMPTY;
        while (pieceBitboard) {
            Square square = popLsb(&pieceBitboard);
            SlidingPieceData &slidingPieceData = slidingPieceDataOf<pieceType>(square);
            Bitboard threats = slidingPieceData.magicHashData.calculateSlidingMoves(otherPieces);
            if (threats & bitboardOf(enemyKing)) {
                //check
                if (isCheck) {
                    isDoubleCheck = true;
                } else {
                    isCheck = true;
                    Bitboard checkDirection = slidingPieceData.xrayData.directionTo(bitboardOf(enemyKing));
                    checkEvasionSquares = (checkDirection & threats) | maskOf(square);
                }
                threatsOf(piece) |= slidingPieceData.xrayData.allDirections;
            } else {
                threatsOf(piece) |= threats;

                //check pins
                if (slidingPieceData.xrayData.allDirections & bitboardOf(enemyKing)) {
                    //there are one or more pieces blocking the king. Check for exact pins.
                    Bitboard directionMask = slidingPieceData.xrayData.directionTo(bitboardOf(enemyKing));
                    Bitboard kingBlockersMask = slidingPieceData.magicHashData.calculateSlidingMoves
                            (bitboardOf(enemyKing)) & ~bitboardOf(enemyKing);
                    Bitboard kingBlockers = directionMask & kingBlockersMask & otherPieces;
                    assert(kingBlockers);
                    Square firstBlockerSquare = popLsb(&kingBlockers);
                    if (!kingBlockers) {
                        // there is exactly one piece blocking the king, so it is a pin.
                        pinned |= maskOf(firstBlockerSquare);
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
        threatsOf(queenPiece) = BITBOARD_EMPTY;
        while (queens) {
            Square square = popLsb(&queens);
            SlidingPieceData &rookPieceData = slidingPieceDataOf<PIECE_TYPE_ROOK>(square);
            SlidingPieceData &bishopPieceData = slidingPieceDataOf<PIECE_TYPE_BISHOP>(square);
            Bitboard threats = rookPieceData.magicHashData.calculateSlidingMoves(otherPieces) |
                               bishopPieceData.magicHashData.calculateSlidingMoves(otherPieces);
            if (threats & bitboardOf(enemyKing)) {
                //check
                if (isCheck) {
                    isDoubleCheck = true;
                } else {
                    isCheck = true;
                    if (rookPieceData.xrayData.allDirections & bitboardOf(enemyKing)) {
                        Bitboard checkDirection = rookPieceData.xrayData.directionTo(bitboardOf(enemyKing));
                        checkEvasionSquares = (checkDirection & threats) | maskOf(square);
                    } else {
                        Bitboard checkDirection = bishopPieceData.xrayData.directionTo(bitboardOf(enemyKing));
                        checkEvasionSquares = (checkDirection & threats) | maskOf(square);
                    }
                }
                threatsOf(queenPiece) |= rookPieceData.xrayData.allDirections |
                                         bishopPieceData.xrayData.allDirections;
            } else {
                threatsOf(queenPiece) |= threats;
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
                    assert(kingBlockers);
                    Square firstBlockerSquare = popLsb(&kingBlockers);
                    if (!kingBlockers) {
                        // there is exactly one piece blocking the king, so it is a pin.
                        pinned |= maskOf(firstBlockerSquare);
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
        threatsOf(kingPiece) = kingMovesFrom(kingSquare);
    }

    template<Player player>
    inline void ChessBoard::calculateThreats() {
        isCheck = false;
        isDoubleCheck = false;
        checkEvasionSquares = BITBOARD_FULL;
        pinned = BITBOARD_EMPTY;

        calculatePawnThreats<player>();
        calculateKnightThreats<player>();
        calculateSlidingPieceThreats<player, PIECE_TYPE_BISHOP>();
        calculateSlidingPieceThreats<player, PIECE_TYPE_ROOK>();
        calculateQueenThreats<player>();
        calculateKingThreats<player>();

        // todo: optimize. maybe have each function return the threats, instead of fetching them directly.
        /// See if that's faster. Or maybe use a for loop

        constexpr Piece pawnPiece = makePiece(PIECE_TYPE_PAWN, player);
        constexpr Piece knightPiece = makePiece(PIECE_TYPE_KNIGHT, player);
        constexpr Piece bishopPiece = makePiece(PIECE_TYPE_BISHOP, player);
        constexpr Piece rookPiece = makePiece(PIECE_TYPE_ROOK, player);
        constexpr Piece queenPiece = makePiece(PIECE_TYPE_QUEEN, player);
        constexpr Piece kingPiece = makePiece(PIECE_TYPE_KING, player);

        threatsOf(player) = threatsOf(pawnPiece) | threatsOf(knightPiece) | threatsOf(bishopPiece) |
                            threatsOf(rookPiece) | threatsOf(queenPiece) | threatsOf(kingPiece);
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
        isCheck = other.isCheck;
        isDoubleCheck = other.isDoubleCheck;

        whiteMayCastleKingSide = other.whiteMayCastleKingSide;
        whiteMayCastleQueenSide = other.whiteMayCastleQueenSide;
        blackMayCastleKingSide = other.blackMayCastleKingSide;
        blackMayCastleQueenSide = other.blackMayCastleQueenSide;

        evalData = other.evalData;
        enPassantSquare = other.enPassantSquare;
        currentPlayer = other.currentPlayer;
        checkEvasionSquares = other.checkEvasionSquares;
        pinned = other.pinned;

        for (Piece piece = PIECE_FIRST;piece<=PIECE_LAST_NOT_EMPTY;++piece){
            pieceBitboards[piece] = other.pieceBitboards[piece];
            threatsByPiece[piece] = other.threatsByPiece[piece];
        }

        pieceBitboards[PIECE_NONE] = other.pieceBitboards[PIECE_NONE];

        byPlayerBitboards[WHITE] = other.byPlayerBitboards[WHITE];
        byPlayerBitboards[BLACK] = other.byPlayerBitboards[BLACK];

        threatsBypLayer[WHITE] = other.threatsBypLayer[WHITE];
        threatsBypLayer[BLACK] = other.threatsBypLayer[BLACK];

        for(Square square = SQ_FIRST;square<=SQ_LAST;++square){
            pieceBitboards[square] = other.pieceBitboards[square];
        }

        return *this;
    }

#define PRINT_IF_FALSE(VAR, VAR_NAME) \
    if(!(VAR)){                             \
        cout << "Falied " << (VAR_NAME) << " \n";            \
    }

    bool ChessBoard::samePositionAs(const ChessBoard &other) {
        const bool whiteMayCastleKingSideEqual = whiteMayCastleKingSide == other.whiteMayCastleKingSide;
        const bool whiteMayCastleQueenSideEqual = whiteMayCastleQueenSide == other.whiteMayCastleQueenSide;
        const bool blackMayCastleKingSideEqual = blackMayCastleKingSide == other.blackMayCastleKingSide;
        const bool blackMayCastleQueenSideEqual = blackMayCastleQueenSide == other.blackMayCastleQueenSide;
        const bool currentPlayerEquals = currentPlayer == other.currentPlayer;
        const bool pieceBitboardsNoneEquals = pieceBitboards[PIECE_NONE] == other.pieceBitboards[PIECE_NONE];
        const bool bypLayerBitboardWhiteEquals = byPlayerBitboards[WHITE] == other.byPlayerBitboards[WHITE];
        const bool byPlayerBitboardsBlackEquals = byPlayerBitboards[BLACK] == other.byPlayerBitboards[BLACK];

        PRINT_IF_FALSE(whiteMayCastleKingSideEqual, "whiteMayCastleKingSideEqual");
        PRINT_IF_FALSE(whiteMayCastleQueenSideEqual, "whiteMayCastleQueenSideEqual");
        PRINT_IF_FALSE(blackMayCastleKingSideEqual, "blackMayCastleKingSideEqual");
        PRINT_IF_FALSE(blackMayCastleQueenSideEqual, "blackMayCastleQueenSideEqual");
        PRINT_IF_FALSE(currentPlayerEquals, "currentPlayerEquals");
        PRINT_IF_FALSE(pieceBitboardsNoneEquals, "pieceBitboardsNoneEquals");
        PRINT_IF_FALSE(bypLayerBitboardWhiteEquals, "bypLayerBitboardWhiteEquals");
        PRINT_IF_FALSE(byPlayerBitboardsBlackEquals,"byPlayerBitboardsBlackEquals");


        if      (!(whiteMayCastleKingSideEqual &&
                   whiteMayCastleQueenSideEqual &&
                   blackMayCastleKingSideEqual &&
                   blackMayCastleQueenSideEqual &&
                   currentPlayerEquals &&
                   pieceBitboardsNoneEquals &&
                   bypLayerBitboardWhiteEquals &&
                   byPlayerBitboardsBlackEquals)) {
            cout << "Failed v2\n";
            return false;
        }

        for (Piece piece = PIECE_FIRST; piece <= PIECE_LAST_NOT_EMPTY; ++piece) {
            if (pieceBitboards[piece] != other.pieceBitboards[piece]) {
                cout << "Piece Bitboard unequal: " << toChar(piece) << "\n";
                return false;
            }
        }

        for(Square square = SQ_FIRST;square<=SQ_LAST;++square){
            if(piecesBySquare[square] != other.piecesBySquare[square]){
                cout << "Piece on Square unequal: " << toString(square) << "\n";
                return false;
            }
        }

        return true;
    }
}