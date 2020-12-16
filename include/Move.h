//
// Created by guykn on 12/9/2020.
//

#ifndef CHESS_MOVE_H
#define CHESS_MOVE_H


#include "types.h"
#include "MoveInputData.h"

using std::string;
namespace Chess {
    struct Move {
        const Square srcSquare;
        const Square dstSquare;
        const Piece srcPiece;
        const Piece dstPiece;

        friend std::ostream &operator<<(std::ostream &os, Move& move);

        // returns true if the move's destination and source are both real squares. does not talk about move legality.
        inline bool isOk() const;

        Move(const Square src, const Square dst, const Piece srcPiece, const Piece dstPiece);

        bool operator==(Move move) const;

        bool matchesMoveInput(MoveInputData moveInputData) const;

        static Move invalidMove;


    };
}

#endif //CHESS_MOVE_H
