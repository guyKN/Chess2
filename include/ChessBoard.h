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

        GameHistory *gameHistory;

        EvalData evalData;

        inline Bitboard &bitboardOf(Piece piece) {
            return pieceBitboards[piece];
        }

        inline Bitboard &bitboardOf(Player player) {
            return byPlayerBitboards[player];
        }

        inline Piece &pieceOn(Square square) {
            return piecesBySquare[square];
        }

        inline Bitboard getBitboardOf(Piece piece) const {
            return pieceBitboards[piece];
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
        void generateMovesForPlayer(MoveList &moveList);

        void calculateInactivePlayerThreats();

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

        bool noPieceOverlap();

        bool noColorOverlap();

        void updateCastling(Move &move);

    public:

        inline ChessBoard(const Piece piecesBySquare[NUM_SQUARES], Player currentPlayerColor) :
                currentPlayer(currentPlayerColor) {
            gameHistory = new GameHistory;
            evalData = EvalData();
            setPosition(piecesBySquare, currentPlayerColor);
        }

        ChessBoard(ChessBoard const &) = delete;

        inline ~ChessBoard() {
            delete gameHistory;
        }

        inline ChessBoard() {
            gameHistory = new GameHistory;
            resetPosition();
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

        void printBitboards();

        inline bool isLegalMoveStart(Square square) {
            Piece piece = getPieceOn(square);
            return (piece != PIECE_NONE) && (playerOf(piece) == currentPlayer);
        }

        void doMove(Move move);

        void doGameMove(Move move);

        inline Piece getPieceOn(Square square) const {
            return piecesBySquare[square];
        }

        inline Bitboard getThreats() {
            return threatsOf(~currentPlayer);
        }

        inline Bitboard getCheckEvasions() {
            return checkEvasionSquares;
        }

        friend std::ostream &operator<<(std::ostream &os, const ChessBoard &chessBoard);

        void assertOk();

        //returns true if the board has no overlap of pieces
        bool isOk();

        void generateMoves(MoveList &moveList);

        inline GameHistory *getGameHistory() const {
            return gameHistory;
        }

        WinState checkWinner(MoveList &moveList);

        Score evaluate();
    };
}


#endif //CHESS_CHESSBOARD_H
