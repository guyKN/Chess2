//
// Created by guykn on 12/9/2020.
//

#ifndef CHESS_OLDMOVE_H
#define CHESS_OLDMOVE_H


#include "types.h"
#include "MoveInputData.h"
#include "Bitboards.h"

using std::string;
namespace Chess {
    struct OldMove {
        Square srcSquare;
        Square dstSquare;
        Piece srcPiece;
        Piece dstPiece;
        CastlingType castlingType;
        Square pawnForward2Square;
        bool isEnPassant;
        Piece promotionPiece; // When there is a promotionBits, this piece is the piece that will be promoted into.
        // When there isn't a promotionBits, this is the same as srcPiece

        std::ostream &longNotation(std::ostream &os) const;

        friend std::ostream &operator <<(std::ostream &os, const OldMove& move){
            return os << toString(move.srcSquare) << toString(move.dstSquare);
        }

        // returns true if the bestMove_'s destination and source are both real squares. does not talk about bestMove_ legality.
        bool isOk() const;

        OldMove() = default;

        OldMove(const Square srcSquare,
             const Square dstSquare,
             const Piece srcPiece,
             const Piece dstPiece) :
                srcSquare(srcSquare),
                dstSquare(dstSquare),
                srcPiece(srcPiece),
                dstPiece(dstPiece),
                promotionPiece(srcPiece),
                castlingType(CASTLE_NONE),
                pawnForward2Square(SQ_INVALID),
                isEnPassant(false) {}

        inline static OldMove promotion(const Square srcSquare,
                                     const Square dstSquare,
                                     const Piece srcPiece,
                                     const Piece dstPiece,
                                     const Piece promotionPiece) {
            return OldMove(srcSquare, dstSquare, srcPiece, dstPiece, promotionPiece, CASTLE_NONE, SQ_INVALID, false);
        }

        inline static OldMove pawnForward2(const Square srcSquare,
                                        const Square dstSquare,
                                        const Piece srcPiece,
                                        const Piece dstPiece) {
            return OldMove(srcSquare, dstSquare, srcPiece, dstPiece, srcPiece, CASTLE_NONE, dstSquare, false);
        }

        inline static OldMove enPassant(const Square srcSquare,
                                     const Square dstSquare,
                                     const Piece srcPiece,
                                     const Piece dstPiece) {
            return OldMove(srcSquare, dstSquare, srcPiece, dstPiece, srcPiece, CASTLE_NONE, SQ_INVALID, true);
        }


        bool operator==(const OldMove &move) const;

        inline bool operator!=(const OldMove &move) const{
            return !(*this==move);
        }

        bool matchesMoveInput(MoveInputData moveInputData) const;

        static OldMove invalidMove;

        static inline OldMove fromCastlingType(CastlingType castlingType) {
            return castleMoves[castlingType];
        }

    private:
        inline OldMove(const Square srcSquare, const Square dstSquare, const Piece srcPiece, const Piece dstPiece,
                    const Piece promotionPiece, const CastlingType castlingType, const Square pawnForward2piece,
                    const bool isEnPassant) :
                srcSquare(srcSquare),
                dstSquare(dstSquare),
                srcPiece(srcPiece),
                dstPiece(dstPiece),
                promotionPiece(promotionPiece),
                castlingType(castlingType),
                pawnForward2Square(pawnForward2piece),
                isEnPassant(isEnPassant) {}

        explicit OldMove(const CastlingData &castlingData);

        static const OldMove castleMoves[NUM_CASTLE_TYPES];
    };
}

#endif //CHESS_OLDMOVE_H
