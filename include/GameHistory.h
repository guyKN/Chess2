//
// Created by guykn on 12/25/2020.
//

#ifndef CHESS_GAMEHISTORY_H
#define CHESS_GAMEHISTORY_H

#include <vector>
#include <ostream>
#include "Move.h"

using std::vector;
namespace Chess {
    class GameHistory {
        vector<Move> moves;
    public:
        void addMove(Move move);
        inline void clear(){
            moves.clear();
        }
        friend ostream &operator<<(ostream &os, const GameHistory &history);
    };
}


#endif //CHESS_GAMEHISTORY_H
