//
// Created by guykn on 12/7/2020.
//

#ifndef CHESS_CHESSBOARD_H
#define CHESS_CHESSBOARD_H


#include "types.h"
#include "Move.h"
#include "MoveInputData.h"
#include "MoveList.h"
#include "GameHistory.h"
#include "EvalData.h"

using std::cout;
namespace Chess {

    struct MoveRevertData {
        bool whiteMayCastleKingSide;
        bool whiteMayCastleQueenSide;
        bool blackMayCastleKingSide;
        bool blackMayCastleQueenSide;

        Square enPassantSquare;
    };

    //todo: expirement with additional representations in addition to bitboards
    // specificly: pieceLists as vectors, and an array of all pieces by squarew

    class ChessBoard {
        Bitboard pieceBitboards[NUM_PIECES] = {};
        Bitboard byPlayerBitboards[NUM_PLAYERS] = {};

        //todo: have threats just for pieces of the inactive player, to save memory
        // (but remember that this may harm the value of pieces for static move evaluation)
        Bitboard threatsByPiece[NUM_NON_EMPTY_PIECES] = {};
        Bitboard threatsBypLayer[NUM_PLAYERS] = {};

        Bitboard checkEvasionSquares = BITBOARD_FULL;
        Bitboard pinned = BITBOARD_EMPTY;

        Piece piecesBySquare[NUM_SQUARES] = {};
        Player currentPlayer = WHITE;

        Square enPassantSquare = SQ_INVALID;

        bool isCheck;
        bool isDoubleCheck;

        bool whiteMayCastleKingSide;
        bool whiteMayCastleQueenSide;
        bool blackMayCastleKingSide;
        bool blackMayCastleQueenSide;

        EvalData evalData;

        static constexpr bool DISABLE_SPECIAL_MOVES = true;

        inline Bitboard &bitboardOf(Piece piece) {
            return pieceBitboards[piece];
        }

        inline Bitboard &bitboardOf(Player player) {
            return byPlayerBitboards[player];
        }

        inline Piece &pieceOn(Square square) {
            return piecesBySquare[square];
        }


        /// used when there wasn't previously a piece on a square, and now there needs to be one
        inline void placePieceOn(Square square,
                                 Piece newPiece, Player newPlayer) {
            SquareMask squareMask = maskOf(square);
            pieceOn(square) = newPiece;
            bitboardOf(newPiece) |= squareMask;
            bitboardOf(newPlayer) |= squareMask;

            bitboardOf(PIECE_NONE) &= ~squareMask;
        }

        /// used when there was a piece on a square, and now the opposite piece is there
        inline void changePieceOn(Square square,
                                  Piece prevPiece,
                                  Piece newPiece, Player newPlayer) {
            SquareMask squareMask = maskOf(square);
            assert(pieceOk(newPiece) && newPiece != PIECE_NONE);
            pieceOn(square) = newPiece;

            bitboardOf(newPiece) |= squareMask;
            bitboardOf(newPlayer) |= squareMask;

            bitboardOf(prevPiece) &= ~squareMask;
            bitboardOf(~newPlayer) &= ~squareMask;
        }

        /// used when there previously a peice on a square, and now there isn't
        inline void removePieceFrom(Square square,
                                    Piece prevPiece, Player prevPlayer) {
            SquareMask squareMask = maskOf(square);
            pieceOn(square) = PIECE_NONE;

            bitboardOf(PIECE_NONE) |= squareMask;

            bitboardOf(prevPiece) &= ~squareMask;
            bitboardOf(prevPlayer) &= ~squareMask;
        }

        /// used the same as changePieceOn, but only when prevPiece may or may not be PIECE_NONE
        /// this assumes that the player of the piece being removed is the opposite player of the piece being placed
        inline void setPieceOn(Square square,
                               Piece prevPiece,
                               Piece newPiece, Player newPlayer) {
            SquareMask squareMask = maskOf(square);
            pieceOn(square) = newPiece;

            bitboardOf(newPiece) |= squareMask;
            bitboardOf(newPlayer) |= squareMask;

            bitboardOf(prevPiece) &= ~squareMask;
            if (prevPiece != PIECE_NONE) {
                bitboardOf(~newPlayer) &= ~squareMask;
            }
        }

        /// same as setPieceOn, except that prevPiece must not be pieceNone, but newPiece may or may not be
        inline void setPieceOn2(Square square,
                                Piece prevPiece, Player prevPlayer,
                                Piece newPiece) {
            SquareMask squareMask = maskOf(square);
            pieceOn(square) = newPiece;

            bitboardOf(newPiece) |= squareMask;
            if (newPiece != PIECE_NONE) {
                bitboardOf(~prevPlayer) |= squareMask;
            }

            bitboardOf(prevPiece) &= ~squareMask;
            bitboardOf(prevPlayer) &= ~squareMask;
        }


        inline Bitboard getBitboardOf(Piece piece) const {
            return pieceBitboards[piece];
        }

        inline Bitboard getBitboardOf(Player player) const {
            return byPlayerBitboards[player];
        }

        inline Bitboard &threatsOf(Player player) {
            return threatsBypLayer[player];
        }

        inline Bitboard &threatsOf(Piece piece) {
            assert(piece != PIECE_NONE);
            return threatsByPiece[piece];
        }

        inline void swapPlayer() {
            currentPlayer = ~currentPlayer;
        }


        template<Player player>
        bool mayCastleKingSide() {
            if constexpr (player == WHITE) {
                return whiteMayCastleKingSide;
            } else {
                return blackMayCastleKingSide;
            }
        }

        template<Player player>
        bool mayCastleQueenSide() {
            if constexpr (player == WHITE) {
                return whiteMayCastleQueenSide;
            } else {
                return blackMayCastleQueenSide;
            }
        }

        inline MoveRevertData getMoveRevertData() {
            return MoveRevertData{whiteMayCastleKingSide, whiteMayCastleQueenSide,
                                  blackMayCastleKingSide, blackMayCastleQueenSide, enPassantSquare};
        }

        template<Player player>
        void generateAllPieces(MoveList &moveList, Bitboard source, Bitboard target);

        template<Player player>
        void generatePawnMoves(MoveList &moveList, Bitboard source, Bitboard target);

        template<Player player>
        void generateKnightMoves(MoveList &moveList, Bitboard source, Bitboard target);

        template<Player player, PieceType pieceType>
        void generateSlidingPieceMoves(MoveList &moveList, Bitboard source, Bitboard target);

        template<Player player>
        void generateKingMoves(MoveList &moveList);

        template<Player player>
        GameEndState generateMovesForPlayer(MoveList &moveList);

        template<Player player>
        void calculateThreats();

        template<Player player>
        void calculatePawnThreats();

        template<Player player>
        void calculateKnightThreats();

        template<Player player, PieceType pieceType>
        void calculateSlidingPieceThreats();

        template<Player player>
        void calculateQueenThreats();

        template<Player player>
        void calculateKingThreats();

        bool noPieceOverlap() const;

        bool noColorOverlap() const;

        void updateCastling(const Move &move);

        void updateBitboards();

        void updateBitboardsForPlayer(Player player);

    public:

        inline ChessBoard(const Piece piecesBySquare[NUM_SQUARES], Player currentPlayerColor) :
                currentPlayer(currentPlayerColor) {
            evalData = EvalData();
            setPosition(piecesBySquare, currentPlayerColor);
        }

        inline ChessBoard() {
            resetPosition();
        }


        ChessBoard &operator=(const ChessBoard &other);

        inline ChessBoard(ChessBoard const &other) {
            *this = other;
        }

        inline void resetPosition() {
            setPosition(startingBoard, STARTING_PLAYER);
        }

        void setPosition(const Piece piecesArray[NUM_SQUARES], Player currentPlayer);

        inline Player getCurrentPlayer() {
            return currentPlayer;
        }

        inline bool getIsCheck() {
            return isCheck;
        }

        inline Bitboard getBitboardOf(Piece piece) {
            return bitboardOf(piece);
        }

        inline Bitboard getPinned() {
            return pinned;
        }

        const static Piece startingBoard[NUM_SQUARES];

        void printBitboards() const;

        inline bool isLegalMoveStart(Square square) {
            Piece piece = getPieceOn(square);
            return (piece != PIECE_NONE) && (playerOf(piece) == currentPlayer);
        }

        MoveRevertData doMove(const Move &move);

        void undoMove(Move &move, MoveRevertData &moveRevertData);

        inline const Piece &getPieceOn(Square square) const {
            return piecesBySquare[square];
        }

        inline Bitboard getThreats() {
            return threatsOf(~currentPlayer);
        }

        inline Bitboard getCheckEvasions() {
            return checkEvasionSquares;
        }

        friend std::ostream &operator<<(std::ostream &os, const ChessBoard &chessBoard);

        void assertOk() const;

        GameEndState generateMoves(MoveList &moveList);

        Score evaluateWhite();

        inline Score evaluate() {
            return evaluateWhite() * currentPlayer;
        }

        void revertTo(const MoveRevertData &moveRevertData);

        bool piecesMatchArray() const;

        bool playerColorsMatch() const;

        bool isOk() const;

        bool samePositionAs(const ChessBoard &other);

        bool parseFen(std::string &fenString);

        ostream & getFen(ostream &outputStream) const;
    };
}




#endif //CHESS_CHESSBOARD_H
