//
// Created by guykn on 1/9/2021.
//

#include "NewMove.h"
namespace Chess{
    const NewMove NewMove::castlingMoves[NUM_CASTLE_TYPES] = {castle_slow(CASTLE_WHITE_KING_SIDE),
                                                              castle_slow(CASTLE_WHITE_QUEEN_SIDE),
                                                              castle_slow(CASTLE_BLACK_KING_SIDE),
                                                              castle_slow(CASTLE_BLACK_QUEEN_SIDE)};

    constexpr NewMove NewMove::castle_slow(CastlingType castlingType) {
        const CastlingData &castlingData = CastlingData::fromCastlingType(castlingType);
        Square src = castlingData.kingSrc;
        Square dst = castlingData.kingDst;
        return NewMove(src | (dst << dstShift) | castlingBits | (castlingType << extraCodeShift));
    }



}