//
// Created by guykn on 12/7/2020.
//

#include <string>
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

    ChessBoard::ChessBoard(const Piece piecesBySquare[NUM_SQUARES], Player currentPlayerColor) :
            currentPlayer(currentPlayerColor) {

        gameHistory = new GameHistory;

        for (Piece piece = PIECE_FIRST; piece <= PIECE_LAST_NOT_EMPTY; ++piece) {
            bitboardOf(piece) = BITBOARD_EMPTY;
            threatsOf(piece) = BITBOARD_EMPTY;
        }
        bitboardOf(PIECE_NONE) = BITBOARD_EMPTY;

        bitboardOf(WHITE) = BITBOARD_EMPTY;
        threatsOf(WHITE) = BITBOARD_EMPTY;
        bitboardOf(BLACK) = BITBOARD_EMPTY;
        threatsOf(BLACK) = BITBOARD_EMPTY;


        SquareMask squareMask = SQUARE_MASK_FIRST;
        for (Square square = SQ_FIRST; square <= SQ_LAST; ++square) {
            Piece piece = piecesBySquare[square];
            pieceOn(square) = piece;
            bitboardOf(piece) |= squareMask;
            if (piece != PIECE_NONE) {
                bitboardOf(playerOf(piece)) |= squareMask;
            }
            squareMask <<= 1;
        }
    }

    ChessBoard::~ChessBoard() {
        delete gameHistory;
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

    void ChessBoard::printBitboards() {
        cout << "\nCurrent player: " << currentPlayer;

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
    }

    //todo: make this not run in release mode
    void ChessBoard::assertOk() {
        if (!isOk()) {
            printBitboards();
            assert(false);
        }
    }

    void ChessBoard::doMove(Move move) {
        //todo: see if execution time changes between making masks once as variable or as temp objects
        assert(move.isOk());
        assert(move.dstPiece == pieceOn(move.dstSquare));
        enPassantSquare = move.pawnForward2Square;
        bitboardOf(PIECE_NONE) |= maskOf(move.srcSquare);
        (bitboardOf(move.srcPiece) &= notSquareMask(move.srcSquare));
        (bitboardOf(move.dstPiece)) &= notSquareMask(move.dstSquare);
        bitboardOf(move.promotionPiece) |= maskOf(move.dstSquare);
        ((bitboardOf(currentPlayer)) &= notSquareMask(move.srcSquare)) |= maskOf(move.dstSquare);
        bitboardOf(~currentPlayer) &= notSquareMask(move.dstSquare);

        pieceOn(move.srcSquare) = PIECE_NONE;
        pieceOn(move.dstSquare) = move.promotionPiece;

        if (move.castlingType != CASTLE_NONE) {
            Piece rook = makePiece(PIECE_TYPE_ROOK, currentPlayer);
            CastlingData castlingData = CastlingData::fromCastlingType(move.castlingType);

            (bitboardOf(rook) &= notSquareMask(castlingData.rookSrc)) |= maskOf(castlingData.rookDst);
            ((bitboardOf(PIECE_NONE)) &= notSquareMask(castlingData.rookDst)) |= maskOf(castlingData.rookSrc);

            ((bitboardOf(currentPlayer)) &= notSquareMask(castlingData.rookSrc)) |= maskOf(castlingData.rookDst);

            pieceOn(castlingData.rookSrc) = PIECE_NONE;
            pieceOn(castlingData.rookDst) = rook;
        }

        // update castling legality
        updateCastling(move);

        swapPlayer();
        calculateInactivePlayerThreats();
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

        bool disableCastlingQueenSide = CastlingData::fromCastlingType(CASTLE_BLACK_QUEEN_SIDE)
                .moveDisablesCastling(moveSquares);
        if(disableCastlingQueenSide){
            cout << "disabled castling queen side\n";
        }
        blackMayCastleQueenSide = blackMayCastleQueenSide &&
                                  !disableCastlingQueenSide;
    }

    void ChessBoard::doGameMove(Move move) {
        doMove(move);
        gameHistory->addMove(move);
    }

    //todo: only call this when assertions are enabled
    bool ChessBoard::isOk() {
        bool noPieceOverlap1 = noPieceOverlap();
        bool noColorOverlap1 = noColorOverlap();
        if (!noPieceOverlap1)
            cout << "Piece overlap \n";
        if (!noColorOverlap1)
            cout << "Color overlap \n";

        return noPieceOverlap1 && noColorOverlap1;
    }

    bool ChessBoard::noPieceOverlap() {
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

    bool ChessBoard::noColorOverlap() {
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


    //todo: optimize templates, see if there's a way to make more stuff inline.
    /// maybe also get rid of template<Player player>, since it just makes functions bigger, and harder to all keep in the cache

    template<Player player>
    void ChessBoard::generatePawnMoves(MoveList &moveList, Bitboard source, Bitboard target) {
        // note: promotionPiece is the piece that the pawns promote to, but it may also be PIECE_PAWN, which means that the pawns don't promote
        // only make promotionPiece into queen when it is know the pawns will promote this move
        constexpr int direction = directionOf(player);
        constexpr int forward1shift = direction * 8;
        constexpr int forward2shift = 2 * forward1shift;

        constexpr int captureLeftShift = forward1shift-1;
        constexpr int captureRightShift = forward1shift+1;
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
        while (forward1Move){
            Square dst = popLsb(&forward1Move);
            moveList.addMove(Move(dst-forward1shift, dst, pawnPiece, pieceOn(dst)));
        }

        //move forward 2
        Bitboard forward2Move = signedShift<forward2shift>(pawns & rank2mask) & emptySquares;
        while (forward2Move){
            Square dst = popLsb(&forward2Move);
            moveList.addMove(
                    Move::pawnForward2(dst-forward2shift, dst, pawnPiece, pieceOn(dst))
            );
        }

        //capture left
        Bitboard captureLeft = signedShift<captureLeftShift>(pawns & ~rank7mask & shiftLeftMask) & enemyPieces;
        while (captureLeft){
            Square dst = popLsb(&captureLeft);
            moveList.addMove(Move(dst-captureLeftShift, dst, pawnPiece, pieceOn(dst)));
        }

        //capture right
        Bitboard captureRight = signedShift<captureRightShift>(pawns & ~rank7mask & shiftRightMask) & enemyPieces;
        while (captureRight){
            Square dst = popLsb(&captureRight);
            moveList.addMove(Move(dst-captureRightShift, dst, pawnPiece, pieceOn(dst)));
        }

        // en passent capture left


        // promotion forward
        Bitboard promotionForward = signedShift<forward1shift>(pawns & rank7mask) & emptySquares;
        while (promotionForward){
            Square dst = popLsb(&promotionForward);
            moveList.addMove(
                    Move::promotion(dst - forward1shift, dst, pawnPiece, pieceOn(dst), queen)
            );
        }

        //promotion left
        Bitboard promotionLeft = signedShift<captureLeftShift>(pawns & rank7mask & shiftLeftMask) & enemyPieces;
        while (promotionLeft){
            Square dst = popLsb(&promotionLeft);
            moveList.addMove(
                    Move::promotion(dst-captureLeftShift, dst, pawnPiece, pieceOn(dst), queen)
            );
        }

        //promotion right
        Bitboard promotionRight = signedShift<captureRightShift>(pawns & rank7mask & shiftRightMask) & enemyPieces;
        while (promotionRight){
            Square dst = popLsb(&promotionRight);
            moveList.addMove(
                    Move::promotion(dst-captureRightShift, dst, pawnPiece, pieceOn(dst), queen)
            );
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
    void ChessBoard::generateMovesForPlayer(MoveList &moveList) {
        generateKingMoves<player>(moveList);
        if (!isDoubleCheck) {
            Bitboard notPinned = ~pinned;
            generateAllPieces<player>(moveList, notPinned, checkEvasionSquares);
            if (!isCheck) {
                Bitboard pieces = bitboardOf(player) | bitboardOf(~player); //todo: speed up
                Bitboard threats = threatsOf(~player);

                if (mayCastleKingSide<player>() &&
                    CastlingData::kingSideCastleOf<player>().mayCastle(pieces, threats)) {
                    cout << player << " Allowed Kingside\n";
                    moveList.addMove(Move::fromCastlingType(kingSideCastleOf<player>()));
                } else if (mayCastleKingSide<player>()) {
                    cout << player << " future Kingside\n";
                } else {
                    cout << player << " disabled Kingside\n";
                }

                if (mayCastleQueenSide<player>() &&
                    CastlingData::queenSideCastleOf<player>().mayCastle(pieces, threats)) {
                    cout << player << " allowed Queenside\n";
                    moveList.addMove(Move::fromCastlingType(queenSideCastleOf<player>()));
                } else if (mayCastleQueenSide<player>()) {
                    cout << player << " future Queenside\n";
                } else {
                    cout << player << " disabled Queenside\n";
                }


                if (pinned) {
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

    void ChessBoard::generateMoves(MoveList &moveList) {
        if (currentPlayer == WHITE) {
            generateMovesForPlayer<WHITE>(moveList);
        } else {
            generateMovesForPlayer<BLACK>(moveList);
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

    void ChessBoard::calculateInactivePlayerThreats() {
        isCheck = false;
        isDoubleCheck = false;
        checkEvasionSquares = BITBOARD_FULL; //todo: remove
        pinned = BITBOARD_EMPTY;
        if (currentPlayer == WHITE) {
            calculateThreats<BLACK>();
        } else {
            calculateThreats<WHITE>();
        }
    }
}