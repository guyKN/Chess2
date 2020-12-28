//
// Created by guykn on 12/28/2020.
//

#ifndef CHESS_EVALDATA_H
#define CHESS_EVALDATA_H

#include <random>
#include <types.h>
namespace Chess {

    struct EvalData;

    struct Scalar {
        int values[Chess::NUM_SQUARES];

        inline const int &operator[](Chess::Square square) const {
            return values[square];
        }

        void addBaseValue(int baseValue);

        void copyFlipped(Scalar &scalar) const;

        int operator*(Chess::Bitboard bitboard) const;
    };


    struct EvalData {
        Scalar pieceSquareValues[NUM_NON_EMPTY_PIECES];

        EvalData();

        void initWhite() {
            for (Piece piece = PIECE_FIRST_BLACK; piece <= PIECE_LAST_BLACK; ++piece) {
                pieceSquareValues[piece].copyFlipped(pieceSquareValues[flip(piece)]);
            }


            std::cout << "White pawns: ";
            printArray((pieceScalar(PIECE_WHITE_PAWN).values), NUM_SQUARES);
            std::cout << "\n";
            std::cout << "Black Pawns: ";
            printArray((pieceScalar(PIECE_BLACK_PAWN).values), NUM_SQUARES) << "\n";
        }

    public:

        inline const Scalar &pieceScalar(Piece piece) {
            assert(piece != PIECE_NONE && piece_ok(piece));
            return pieceSquareValues[piece];
        }
    };
}

#endif //CHESS_EVALDATA_H
