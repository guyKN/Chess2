//
// Created by guykn on 12/7/2020.
//

#ifndef CHESS_CHESSBOARD_H
#define CHESS_CHESSBOARD_H


#include "types.h"
#include "Move.h"
#include "MoveInputData.h"
#include "MoveList.h"

using std::cout;
namespace Chess {

    //todo: expirement with additional representations in addition to bitboards
    // specificly: pieceLists as vectors, and an array of all pieces by squarew

    class ChessBoard {
        Bitboard pieceBitboards[NUM_PIECES] = {};
        Bitboard byPlayerBitboards[NUM_PLAYERS] = {};
        Piece piecesBySquare[NUM_SQUARES] = {};

        Player currentPlayer = WHITE;

        inline Bitboard &bitboardOf(Piece piece) {
            return pieceBitboards[piece];
        }

        inline Bitboard &bitboardOf(Player player){
            return byPlayerBitboards[player];
        }

        inline Piece &pieceOn(Square square){
            return piecesBySquare[square];
        }

        inline Bitboard getBitboardOf(Piece piece) const {
            return pieceBitboards[piece];
        }

        void swapPlayer() {
            currentPlayer = ~currentPlayer;
        }

        template<Player player>
        void generatePawnQuietMoves(MoveList &moveList);

        template<Player, int>
        void generatePawnCaptures(MoveList &moveList);

        template<Player player>
        void generateKnightMoves(MoveList &moveList);

        template<Player player>
        void generateKingMoves(MoveList &moveList);

        template<Player player>
        void generateMovesForPlayer(MoveList &moveList);

        bool noPieceOverlap();

        bool noColorOverlap();

    public:

        inline Player getCurrentPlayer(){
            return currentPlayer;
        }

        const static Piece startingBoard[NUM_SQUARES];

        void printBitboards();

        inline bool isLegalMoveStart(Square square){
            Piece piece = getPieceOn(square);
            return (piece != PIECE_NONE) && (playerOf(piece) == currentPlayer);
        }

        explicit ChessBoard(const Piece piecesBySquare[NUM_SQUARES], Player currentPlayerColor);

        inline ChessBoard() : ChessBoard(startingBoard, STARTING_PLAYER) {}

        void doMove(Move move);

        inline Piece getPieceOn(Square square) const{
            return piecesBySquare[square];
        }

        friend std::ostream &operator<<(std::ostream &os, const ChessBoard &chessBoard);

        void assertOk();

        //returns true if the board has no overlap of pieces
        bool isOk();

        void generateMoves(MoveList &moveList);
    };
}


#endif //CHESS_CHESSBOARD_H
