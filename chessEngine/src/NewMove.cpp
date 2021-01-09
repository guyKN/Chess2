//
// Created by guykn on 1/9/2021.
//

#include "NewMove.h"
namespace Chess{
    const NewMove NewMove::castlingMoves[NUM_CASTLE_TYPES] = {castle_slow(CASTLE_WHITE_KING_SIDE),
                                                              castle_slow(CASTLE_WHITE_QUEEN_SIDE),
                                                              castle_slow(CASTLE_BLACK_KING_SIDE),
                                                              castle_slow(CASTLE_BLACK_QUEEN_SIDE)};

    NewMove NewMove::castle_slow(CastlingType castlingType) {
        const CastlingData &castlingData = CastlingData::fromCastlingType(castlingType);
        Square src = castlingData.kingSrc;
        Square dst = castlingData.kingDst;
        return NewMove(src | (dst << DST_SHIFT) | CASTLING_MOVE | (castlingType << EXTRA_CODE_SHIFT));
    }

    ostream &operator<<(ostream &os, const NewMove &move) {
        os << move.src() << move.dst();
        if(move.moveType() == NewMove::PROMOTION_MOVE){
            os << toChar(move.promotionPieceType());
        }
        return os;
    }


}