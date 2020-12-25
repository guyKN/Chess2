//
// Created by guykn on 12/9/2020.
//

#ifndef CHESS_MOVE_H
#define CHESS_MOVE_H


#include "types.h"
#include "MoveInputData.h"
#include "Bitboards.h"

using std::string;
namespace Chess {
    struct Move {
        const Square srcSquare;
        const Square dstSquare;
        const Piece srcPiece;
        const Piece dstPiece;
        const CastlingType castlingType;

        friend std::ostream &operator<<(std::ostream &os, Move &move);

        // returns true if the move's destination and source are both real squares. does not talk about move legality.
        bool isOk() const;

        Move(const Square srcSquare,
             const Square dstSquare,
             const Piece srcPiece,
             const Piece dstPiece) :
                srcSquare(srcSquare),
                dstSquare(dstSquare),
                srcPiece(srcPiece),
                dstPiece(dstPiece),
                castlingType(CASTLE_NONE) {}


        bool operator==(Move move) const;

        bool matchesMoveInput(MoveInputData moveInputData) const;

        static Move invalidMove;

        static inline Move fromCastlingType(CastlingType castlingType){
            return castleMoves[castlingType];
        }

    private:
        Move(CastlingData castlingData);
        static const Move castleMoves[NUM_CASTLE_TYPES];
    };
}

#endif //CHESS_MOVE_H
