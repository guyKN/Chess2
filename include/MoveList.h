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
        Move moves[MAX_MOVES];
        Move *currentMove = moves;
    public:
        MoveList() = default;
        MoveList(MoveList const &) = delete;

        inline const Move *firstMove() const {
            return moves;
        }

        inline const Move *lastMove() const {
            return currentMove;
        }

        inline void addMove(Move move) {
            *(currentMove) = move;
            currentMove++;
            assert(size() < MAX_MOVES);
        }

        inline int size() const {
            return currentMove - &moves[0];
        }

        inline bool isEmpty() const {
            return currentMove == moves;
        }

        inline void clear(){
            currentMove = moves;
        }

        Move getMoveFromInputData(MoveInputData moveInput) const;

        inline Move operator[](int index) const {
            return moves[index];
        }

        void movesFrom(Chess::Square square, MoveList &moveList) const;

        friend ostream &operator<<(ostream &os, const MoveList &list);
    };
}

#endif //CHESS_MOVELIST_H
