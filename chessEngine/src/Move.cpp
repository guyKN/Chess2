//
// Created by guykn on 1/9/2021.
//

#include "Move.h"

//todo: have a seprate comparasion for just the first 16 bits

namespace Chess {
    const Move Move::castlingMoves[NUM_CASTLE_TYPES] = {castle_slow(CASTLE_WHITE_KING_SIDE),
                                                        castle_slow(CASTLE_WHITE_QUEEN_SIDE),
                                                        castle_slow(CASTLE_BLACK_KING_SIDE),
                                                        castle_slow(CASTLE_BLACK_QUEEN_SIDE)};

    Move Move::castle_slow(CastlingType castlingType) {
        const CastlingData &castlingData = CastlingData::fromCastlingType(castlingType);
        Square src = castlingData.kingSrc;
        Square dst = castlingData.kingDst;
        return Move(src | (dst << DST_SHIFT) | CASTLING_MOVE | (castlingType << EXTRA_CODE_SHIFT));
    }

    bool Move::matchesMoveInput(MoveInputData moveInputData) const {

        if (moveType() == CASTLING_MOVE) {
            bool isKingSideCastle = isKingSide(castlingType());
            if ((isKingSideCastle && moveInputData.isKingSideCastle) ||
                (!isKingSideCastle && moveInputData.isQueenSideCastle)) {
                return true;
            }
        }

        PieceType promotionPiece = moveType() == PROMOTION_MOVE ? promotionPieceType() : PIECE_TYPE_NONE;
        return moveInputData.src == src() &&
               moveInputData.dst == dst() &&
               ((moveInputData.promotionPiece == promotionPiece) ||
                (moveInputData.promotionPiece == PIECE_TYPE_NONE
                 && promotionPiece == PIECE_TYPE_QUEEN));
    }

    ostream &operator<<(ostream &os, const Move &move) {
        if (move == Move::invalid()) {
            return os << "0000";
        }
        os << move.src() << move.dst();
        if (move.moveType() == Move::PROMOTION_MOVE) {
            os << toChar(move.promotionPieceType());
        }
//        if(move.score() != STATIC_SCORE_ZERO){
//            os << '+' << static_cast<int>(move.score());
//        }
        return os;
    }


}