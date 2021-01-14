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
        Move moves[MAX_MOVES];//todo: remove default initialization!!!
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
            assert(move.isOk());//inside of moveList.h
            *(currentMove) = move;
            currentMove++;
            assert(size() < MAX_MOVES);
        }

        bool contains(Move move) const;

        bool notFirstContains(Move move) const;



        inline void addPromotions(Square src, Square dst){
            // optimize: should inline?
            // optimize: should loop?
            // optimize: should optimize a tiny bit by removing the -1 in promotionMove

            addMove(Move::promotionMove(src, dst, PIECE_TYPE_QUEEN));
            addMove(Move::promotionMove(src, dst, PIECE_TYPE_ROOK));
            addMove(Move::promotionMove(src, dst, PIECE_TYPE_BISHOP));
            addMove(Move::promotionMove(src, dst, PIECE_TYPE_KNIGHT));
        }

        inline unsigned int size() const {
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
