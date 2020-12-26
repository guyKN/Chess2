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
        const Square pawnForward2Square;
        const Piece promotionPiece; // When there is a promotion, this piece is the piece that will be promoted into.
        // When there isn't a promotion, this is the same as srcPiece


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
                promotionPiece(srcPiece),
                castlingType(CASTLE_NONE),
                pawnForward2Square(SQ_INVALID){}

        inline static Move promotion(const Square srcSquare,
                                     const Square dstSquare,
                                     const Piece srcPiece,
                                     const Piece dstPiece,
                                     const Piece promotionPiece){
            return Move(srcSquare, dstSquare, srcPiece, dstPiece, promotionPiece, CASTLE_NONE, SQ_INVALID);
        }

        inline static Move pawnForward2(const Square srcSquare,
                                     const Square dstSquare,
                                     const Piece srcPiece,
                                     const Piece dstPiece){
            return Move(srcSquare, dstSquare, srcPiece, dstPiece, srcPiece, CASTLE_NONE, srcSquare);
        }


        bool operator==(const Move &move) const;

        bool matchesMoveInput(MoveInputData moveInputData) const;

        static Move invalidMove;

        static inline Move fromCastlingType(CastlingType castlingType) {
            return castleMoves[castlingType];
        }

    private:
        inline Move(const Square srcSquare, const Square dstSquare, const Piece srcPiece, const Piece dstPiece,
                    const Piece promotionPiece, const CastlingType castlingType, const Square pawnForward2piece) :
                srcSquare(srcSquare),
                dstSquare(dstSquare),
                srcPiece(srcPiece),
                dstPiece(dstPiece),
                promotionPiece(promotionPiece),
                castlingType(castlingType),
                pawnForward2Square(pawnForward2piece) {}

        Move(CastlingData castlingData);

        static const Move castleMoves[NUM_CASTLE_TYPES];
    };
}

#endif //CHESS_MOVE_H
