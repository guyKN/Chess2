//
// Created by guykn on 1/9/2021.
//

#ifndef CHESS_NEWMOVE_H
#define CHESS_NEWMOVE_H

#include "types.h"
#include "Bitboards.h"

namespace Chess {
    class NewMove {
        /// uses 16 bits
        /// bits 0-5 src square
        /// bits 6-11 dst square
        /// bits 12-13 move code. 0b00=normal move, 0b01=promotionBits, 0b10=castlingBits, 0b11=en passant
        /// bits 14-15: extra special move data.
        /// if promotionBits then:
        ///       0b00 means knight
        ///       0b01 mean bishop
        ///       0b10 mean rook
        ///       0b11 means queen
        /// if castlingBits then corresponds to the castlingBits CastlingType enum
        /// if en passant, then 0b00 means pawn forward 2, 0b01 means en Passant capture
        /// if dst==0, and src==0, then the move is invalid
        unsigned int code;


        constexpr explicit NewMove(unsigned int code) : code(code) {}

        static constexpr int dstShift = 6;
        static constexpr int moveCodeShift = 12;
        static constexpr int extraCodeShift = 14;

        static constexpr unsigned int promotionBits = 1u << moveCodeShift;
        static constexpr unsigned int castlingBits = 2u << moveCodeShift;

        static constexpr int pawnForward2Bits = 3 << moveCodeShift;
        static constexpr int enPassantBits = (3 << moveCodeShift) | (1 << extraCodeShift);

        static constexpr unsigned int;
        
        static constexpr NewMove castle_slow(CastlingType castlingType);

        static const NewMove castlingMoves[NUM_CASTLE_TYPES];

    public:

        Square src(){
            return
        }
        
        static constexpr inline NewMove normalMove(Square src, Square dst) {
            return NewMove(src | (dst << dstShift));
        }

        static constexpr inline NewMove castle(CastlingType castlingType){
            return castlingMoves[castlingType];
        }

        static constexpr inline NewMove promotionMove(Square src, Square dst, PieceType promotionPiece){
            return NewMove(src | (dst << dstShift) | promotionBits | (promotionCode(promotionPiece) << extraCodeShift));
        }

        static constexpr inline NewMove pawnForward2(Square src, Square dst){
            return NewMove(src | (dst << dstShift) | pawnForward2Bits);
        }

        static constexpr inline NewMove enPassant(Square src, Square dst){
            return NewMove(src | (dst << dstShift) | enPassantBits);
        }
    };
}


#endif //CHESS_NEWMOVE_H
