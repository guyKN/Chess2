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
        vector<Move> internalMoves;
    public:
        vector<Move>& moves = internalMoves;
        inline void addMove(Move move){
            internalMoves.push_back(move);
        }
        inline void pop(){
            internalMoves.pop_back();
        }
        inline void clear(){
            internalMoves.clear();
        }
        friend ostream &operator<<(ostream &os, const GameHistory &history);
    };
}


#endif //CHESS_GAMEHISTORY_H
