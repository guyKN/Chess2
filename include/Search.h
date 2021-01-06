//
// Created by guykn on 12/30/2020.
//

#ifndef CHESS_SEARCH_H
#define CHESS_SEARCH_H

#include "ChessBoard.h"

namespace Chess {
    class Search {

        constexpr static bool PRINT_DEBUG = false;

        ChessBoard chessBoard;

        GameHistory gameHistory_;

        int numNonLeafNodes = 0;
        int numLeaves = 0;

    public:

        const GameHistory &gameHistory = gameHistory_;

        Search(const ChessBoard &chessBoard) : chessBoard(chessBoard) {}

        Search() = default;

        inline Move bestMove(int depth) {
            numNonLeafNodes=0;
            numLeaves=0;
            return alphaBetaRoot(depth);
        }

        inline Search &setPos(const ChessBoard &newChessBoard) {
            this->chessBoard = newChessBoard;
            return *this;
        }

        int getNumLeaves(){
            return numLeaves;
        }
        int getNumNodes(){
            return numNonLeafNodes + numLeaves;
        }


    private:

        Score alphaBeta(Score alpha, Score beta, int depthLeft);

        Move alphaBetaRoot(int depth);
    };
}


#endif //CHESS_SEARCH_H
