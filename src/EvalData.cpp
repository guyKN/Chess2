//
// Created by guykn on 12/28/2020.
//

#include <EvalData.h>
#include <Bitboards.h>
#include <types.h>
namespace Chess {

    void Scalar::copyFlipped(Scalar &scalar) const {
        for (Square square = SQ_FIRST; square <= SQ_LAST; ++square) {
            scalar.values[flip(square)] = this->values[square];
        }
    }

    int Scalar::operator*(Bitboard bitboard) const {
        int val = 0;
        while (bitboard) {
            Square square = popLsb(&bitboard);
            val += values[square];
        }
        return val;
    }

    void Scalar::addBaseValue(int baseValue) {
        for(Square square= SQ_FIRST;square<=SQ_LAST;++square){
            values[square]+=baseValue;
        }
    }

    EvalData::EvalData() {
        pieceSquareValues[PIECE_BLACK_PAWN] = {0, 0, 0, 0, 0, 0, 0, 0,
                                                        50, 50, 50, 50, 50, 50, 50, 50,
                                                        10, 10, 20, 30, 30, 20, 10, 10,
                                                        5, 5, 10, 25, 25, 10, 5, 5,
                                                        0, 0, 0, 20, 20, 0, 0, 0,
                                                        5, -5, -10, 0, 0, -10, -5, 5,
                                                        5, 10, 10, -20, -20, 10, 10, 5,
                                                        0, 0, 0, 0, 0, 0, 0, 0
        };
        pieceSquareValues[PIECE_BLACK_PAWN].addBaseValue(100);

        pieceSquareValues[PIECE_BLACK_KNIGHT] = {
                -50, -40, -30, -30, -30, -30, -40, -50,
                -40, -20, 0, 0, 0, 0, -20, -40,
                -30, 0, 10, 15, 15, 10, 0, -30,
                -30, 5, 15, 20, 20, 15, 5, -30,
                -30, 0, 15, 20, 20, 15, 0, -30,
                -30, 5, 10, 15, 15, 10, 5, -30,
                -40, -20, 0, 5, 5, 0, -20, -40,
                -50, -40, -30, -30, -30, -30, -40, -50,
        };
        pieceSquareValues[PIECE_BLACK_KNIGHT].addBaseValue(320);


        pieceSquareValues[PIECE_BLACK_BISHOP] = {
                -20, -10, -10, -10, -10, -10, -10, -20,
                -10, 0, 0, 0, 0, 0, 0, -10,
                -10, 0, 5, 10, 10, 5, 0, -10,
                -10, 5, 5, 10, 10, 5, 5, -10,
                -10, 0, 10, 10, 10, 10, 0, -10,
                -10, 10, 10, 10, 10, 10, 10, -10,
                -10, 5, 0, 0, 0, 0, 5, -10,
                -20, -10, -10, -10, -10, -10, -10, -20,
        };
        pieceSquareValues[PIECE_BLACK_BISHOP].addBaseValue(330);


        pieceSquareValues[PIECE_BLACK_ROOK] = {
                0, 0, 0, 0, 0, 0, 0, 0,
                5, 10, 10, 10, 10, 10, 10, 5,
                -5, 0, 0, 0, 0, 0, 0, -5,
                -5, 0, 0, 0, 0, 0, 0, -5,
                -5, 0, 0, 0, 0, 0, 0, -5,
                -5, 0, 0, 0, 0, 0, 0, -5,
                -5, 0, 0, 0, 0, 0, 0, -5,
                0, 0, 0, 5, 5, 0, 0, 0
        };

        pieceSquareValues[PIECE_BLACK_ROOK].addBaseValue(500);


        pieceSquareValues[PIECE_BLACK_QUEEN] = {
                -20, -10, -10, -5, -5, -10, -10, -20,
                -10, 0, 0, 0, 0, 0, 0, -10,
                -10, 0, 5, 5, 5, 5, 0, -10,
                -5, 0, 5, 5, 5, 5, 0, -5,
                0, 0, 5, 5, 5, 5, 0, -5,
                -10, 5, 5, 5, 5, 5, 0, -10,
                -10, 0, 5, 0, 0, 0, 0, -10,
                -20, -10, -10, -5, -5, -10, -10, -20
        };

        pieceSquareValues[PIECE_BLACK_QUEEN].addBaseValue(900);

        pieceSquareValues[PIECE_BLACK_KING] = {
                -30, -40, -40, -50, -50, -40, -40, -30,
                -30, -40, -40, -50, -50, -40, -40, -30,
                -30, -40, -40, -50, -50, -40, -40, -30,
                -30, -40, -40, -50, -50, -40, -40, -30,
                -20, -30, -30, -40, -40, -30, -30, -20,
                -10, -20, -20, -20, -20, -20, -20, -10,
                20, 20, 0, 0, 0, 0, 20, 20,
                20, 30, 10, 0, 0, 10, 30, 20
        };

        initWhite();

    }
}
