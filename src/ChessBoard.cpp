//
// Created by guykn on 12/7/2020.
//

#include <string>
#include "ChessBoard.h"
#include "iostream"
#include "Bitboards.h"

namespace Chess {
    ChessBoard::ChessBoard(const Piece piecesBySquare[NUM_SQUARES], Player currentPlayerColor) : currentPlayer(
            currentPlayerColor) {

        for (Piece piece = PIECE_FIRST; piece <= PIECE_LAST; ++piece) {
            bitboardOf(piece) = BITBOARD_EMPTY;
        }

        bitboardOf(WHITE) = BITBOARD_EMPTY;
        bitboardOf(BLACK) = BITBOARD_EMPTY;


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

    void ChessBoard::doMove(Move move) {
        assert(move.isOk());
        bitboardOf(PIECE_NONE) |= maskOf(move.srcSquare);
        (bitboardOf(move.srcPiece) &= notSquareMask(move.srcSquare)) |= maskOf(move.dstSquare);
        (bitboardOf(move.dstPiece)) &= notSquareMask(move.dstSquare);

        ((bitboardOf(currentPlayer)) &= notSquareMask(move.srcSquare)) |= maskOf(move.dstSquare);
        bitboardOf(~currentPlayer) &= notSquareMask(move.dstSquare);

        pieceOn(move.srcSquare) = PIECE_NONE;
        pieceOn(move.dstSquare) = move.srcPiece;

        swapPlayer();
    }


    bool ChessBoard::isOk() {
        return noPieceOverlap() & noColorOverlap();
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


    //todo: optimize templates, see if there's a way to make more stuff inline

    template<Player player>
    void ChessBoard::generatePawnQuietMoves(MoveList &moveList) {
        constexpr int forward1shift = directionOf(player) * 8;
        constexpr int forward4shift = forward1shift * 4;
        constexpr int forward2shift = 2 * forward1shift;
        constexpr Piece pawnPiece = makePiece(PIECE_TYPE_PAWN, player);
        constexpr Rank fourthRank = flipIfBlack(player, RANK_4);
        Bitboard fourthRankMask = maskOfRank(fourthRank);
        Bitboard pawns = bitboardOf(pawnPiece);
        Bitboard emptySquares = bitboardOf(PIECE_NONE);
        Bitboard mayMoveForward2mask = signedShift<forward1shift>(emptySquares) & emptySquares & fourthRankMask;

        //todo: see if this is fastest
        while (pawns) {
            Square srcSquare = popLsb(&pawns);
            SquareMask srcSquareMask = maskOf(srcSquare);
            Bitboard legalMoves = (signedShift<forward1shift>(srcSquareMask) & emptySquares) |
                                  (signedShift<forward2shift>(srcSquareMask) & mayMoveForward2mask);
            while (legalMoves) {
                moveList.addMove(Move(srcSquare, popLsb(&legalMoves), pawnPiece, PIECE_NONE));
            }
        }
    }

    template<Player player, int captureDirection>
    void ChessBoard::generatePawnCaptures(MoveList &moveList) {
        //todo: check if doing this is fastest, or if doing captures both ways together is faster
        constexpr int shift = directionOf(player) * 8 + captureDirection;
        constexpr Piece pawnPiece = makePiece(PIECE_TYPE_PAWN, player);
        Bitboard fileMask = fileShiftMask<captureDirection>();
        constexpr Player opponent = ~player;

        Bitboard validPawns = bitboardOf(pawnPiece) & fileMask; //all pawns, except for pawns on the edge that can't capture
        Bitboard enemyPieces = bitboardOf(opponent);
        while (validPawns) {
            Square srcSquare = popLsb(&validPawns);
            SquareMask srcSquareMask = maskOf(srcSquare);
            Bitboard moveBitboard = signedShift<shift>(srcSquareMask) & enemyPieces;
            if (moveBitboard) {
                Square dstSquare = lsb(moveBitboard);
                moveList.addMove(Move(srcSquare,
                                      dstSquare,
                                      pawnPiece,
                                      pieceOn(dstSquare)));
            }
        }
    }
    template<Player player>
    void ChessBoard::generateKnightMoves(MoveList &moveList){
        constexpr Piece knightPiece = makePiece(PIECE_TYPE_KNIGHT, player);
        Bitboard knights = getBitboardOf(knightPiece);
        Bitboard availableSquares = ~bitboardOf(player);
        while (knights){
            Square srcSquare = popLsb(&knights);
            Bitboard knightMoves = knightMovesFrom(srcSquare) & availableSquares;
            while (knightMoves){
                Square dstSquare = popLsb(&knightMoves);
                moveList.addMove(Move(srcSquare, dstSquare, knightPiece, pieceOn(dstSquare)));
            }
        }
    }

    template<Player player>
    void ChessBoard::generateBishopMoves(MoveList &moveList){
        constexpr Piece bishopPiece = makePiece(PIECE_TYPE_BISHOP, player);
        constexpr Player opponent = ~player;
        Bitboard bishops = getBitboardOf(bishopPiece);
        Bitboard availableSquares = ~bitboardOf(player);
        Bitboard otherPieces = bitboardOf(player) | bitboardOf(opponent);
        while (bishops){
            Square srcSquare = popLsb(&bishops);
            printBitboard(otherPieces);
            Bitboard bishopMoves = bishopMovesFrom(srcSquare, otherPieces & notSquareMask(srcSquare)) & availableSquares;
            while (bishopMoves){
                Square dstSquare = popLsb(&bishopMoves);
                moveList.addMove(Move(srcSquare, dstSquare, bishopPiece, pieceOn(dstSquare)));
            }
        }
    }




    template<Player player>
    void ChessBoard::generateKingMoves(MoveList &moveList) {
        constexpr Piece kingPiece = makePiece(PIECE_TYPE_KING, player);
        Bitboard availableSquares = ~bitboardOf(player);
        Bitboard king = bitboardOf(kingPiece);
        assert(king);
        Square srcSquare = lsb(king);
        Bitboard kingMoves = kingMovesFrom(srcSquare) & availableSquares;
        while (kingMoves){
            Square dstSquare = popLsb(&kingMoves);
            moveList.addMove(Move(srcSquare, dstSquare, kingPiece, pieceOn(dstSquare)));
        }
    }

    template<Player player>
    inline void ChessBoard::generateMovesForPlayer(MoveList &moveList) {
        generatePawnQuietMoves<player>(moveList);
        generatePawnCaptures<player, 1>(moveList);
        generatePawnCaptures<player, -1>(moveList);
        generateKnightMoves<player>(moveList);
        generateBishopMoves<player>(moveList);
        generateKingMoves<player>(moveList);
    }

    void ChessBoard::generateMoves(MoveList &moveList) {
        if (currentPlayer == WHITE) {
            generateMovesForPlayer<WHITE>(moveList);
        } else {
            generateMovesForPlayer<BLACK>(moveList);
        }
    }

    void ChessBoard::printBitboards() {
        cout << "\nCurrent player: " << currentPlayer;
        cout << "\nWhite Pawns:";
        printBitboard(bitboardOf(PIECE_WHITE_PAWN));

        cout << "\nBlack Pawns:";
        printBitboard(bitboardOf(PIECE_BLACK_PAWN));

        cout << "\nWhite Knights";
        printBitboard(bitboardOf(PIECE_WHITE_KNIGHT));

        cout << "\nBlack Knights";
        printBitboard(bitboardOf(PIECE_BLACK_KNIGHT));


        cout << "\nEmpty squares:";
        printBitboard(bitboardOf(PIECE_NONE));

        cout << "\nWhite Pieces:";
        printBitboard(bitboardOf(WHITE));

        cout << "\nBlack Pieces:";
        printBitboard(bitboardOf(BLACK));
    }

    void ChessBoard::assertOk() {
        if (!isOk()) {
            printBitboards();
            assert(false);
        }
    }

    const Piece ChessBoard::startingBoard[] = {PIECE_NONE, PIECE_WHITE_KNIGHT, PIECE_WHITE_BISHOP, PIECE_NONE, PIECE_WHITE_KING, PIECE_WHITE_BISHOP,
                                               PIECE_WHITE_KNIGHT, PIECE_NONE,
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
                                               PIECE_NONE, PIECE_BLACK_KNIGHT, PIECE_BLACK_BISHOP, PIECE_NONE, PIECE_BLACK_KING, PIECE_BLACK_BISHOP,
                                               PIECE_BLACK_KNIGHT, PIECE_NONE};
}