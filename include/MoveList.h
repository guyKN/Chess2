//
// Created by guykn on 12/11/2020.
//

#ifndef CHESS_MOVELIST_H
#define CHESS_MOVELIST_H


#include <vector>
#include <ostream>
#include "Move.h"

using std::vector;
using std::ostream;
using std::cout;
namespace Chess {
    class MoveList {
        vector<Move> moves = vector<Move>();
    public:
        inline void addMove(Move move){
            moves.push_back(move);
        }
        inline int size () const{
            return moves.size();
        }
        Move getMoveFromInputData(MoveInputData moveInput);

        inline Move operator[](int index) const {
            return moves[index];
        }

        void movesFrom(Chess::Square square, MoveList &moveList);

        friend ostream &operator<<(ostream &os, const MoveList &list);
    };
}

#endif //CHESS_MOVELIST_H
