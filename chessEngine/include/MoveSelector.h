//
// Created by guykn on 1/20/2021.
//

#ifndef CHESS_MOVESELECTOR_H
#define CHESS_MOVESELECTOR_H

#include <Move.h>
#include "MoveList.h"
#include "ChessBoard.h"
#include "types.h"
#include "Search.h"

namespace Chess {
    enum Stage {
        TT_MOVE,
        INIT,
        WINNING_CAPTURES,
        NORMAL_MOVES,
        LOSING_CAPTURES,
    };

    constexpr int NUM_STAGES = 6;

    std::ostream& operator<<(std::ostream& os, Stage stage);

    inline bool stageOk(Stage stage){
        return stage >=0 && stage <=LOSING_CAPTURES;
    }

    ENABLE_INCR_OPERATORS_ON(Stage)

    class MoveSelector {
        const Move ttMove;
        ChessBoard &chessBoard;
        MoveChunk moveChunk{};
        Stage stage;
        const Move* currentMove;
        const Search* search;
        const bool doNonCaptures;
        ThreatData threatData{};


    public:
        MoveSelector(ChessBoard &chessBoard, const bool doNonCaptures = true, const Move &ttMove = Move::invalid(), const Search* search = nullptr) :
                ttMove(ttMove), chessBoard(chessBoard),doNonCaptures(doNonCaptures) , stage(TT_MOVE), search(search) {}


        Move nextMove();

        void initWinningCaptures();

        void initLosingCaptures();

        void initNormalMoves();
    };
}

#endif //CHESS_MOVESELECTOR_H
