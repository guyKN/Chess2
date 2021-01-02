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

    public:

        const GameHistory& gameHistory = gameHistory_;

        Search(const ChessBoard &chessBoard) : chessBoard(chessBoard) {}

        Search() = default;

        inline Move bestMove(int depth){
            return alphaBetaRoot(depth);
        }

        inline Search& setPos(const ChessBoard& newChessBoard){
            this->chessBoard = newChessBoard;
            return *this;
        }

    private:

        Score alphaBeta(Score alpha, Score beta, int depthLeft, ChessBoard chessBoard, const Move &move);

        Move alphaBetaRoot(int depth);
    };
}


#endif //CHESS_SEARCH_H
