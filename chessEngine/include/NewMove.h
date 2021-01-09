//
// Created by guykn on 1/9/2021.
//

#ifndef CHESS_NEWMOVE_H
#define CHESS_NEWMOVE_H

#include <ostream>
#include "types.h"
#include "Bitboards.h"
#include "MoveInputData.h"

namespace Chess {

    class NewMove {
        //todo: check the best order for bits. the last and first bits are importantlyly convenient, because they only require a shift or mask operation, not both
        /// uses 16 bits
        /// bits 0-5 src square
        /// bits 6-11 dst square
        /// bits 12-13 move code. 0b00=normal move, 0b01=promotion, 0b10=castling, 0b11=en passant
        /// bits 14-15: extra special move data.
        /// if promotionBits then:
        ///       0b00 means knight
        ///       0b01 mean bishop
        ///       0b10 mean rook
        ///       0b11 means queen
        /// if castlingBits then corresponds to the castlingBits CastlingType enum
        /// if en passant, then 0b00 means pawn forward 2, 0b01 means en Passant capture
        /// if dst==0, and src==0, then the move is invalid
        using codeType = unsigned int;
        codeType code;

        constexpr explicit NewMove(codeType code) : code(code) {}

        static constexpr int DST_SHIFT = 6;
        static constexpr int MOVE_TYPE_SHIFT = 12;
        static constexpr int EXTRA_CODE_SHIFT = 14;

    public:
        enum MoveType : codeType {
            NORMAL_MOVE = 0b00 << MOVE_TYPE_SHIFT,
            PROMOTION_MOVE = 0b01 << MOVE_TYPE_SHIFT,
            CASTLING_MOVE = 0b10 << MOVE_TYPE_SHIFT,
            EN_PASSANT_MOVE = 0b11 << MOVE_TYPE_SHIFT
        };
    private:

        static constexpr codeType SQUARE_BITS = 0b111111;
        static constexpr codeType MOVE_TYPE_BITS = 0b11 << MOVE_TYPE_SHIFT;
        static constexpr codeType EN_PASSANT_CAPTURE = 1 << EXTRA_CODE_SHIFT;

        static NewMove castle_slow(CastlingType castlingType);
        static const NewMove castlingMoves[NUM_CASTLE_TYPES];

        static constexpr inline codeType promotionCode(PieceType pieceType){
            assert(isValidPromotion(pieceType));
            return pieceType-1;
        }

        static constexpr inline PieceType fromPromotionCode(codeType promotionCode){
            return static_cast<PieceType>(promotionCode + 1);
        }


    public:

        inline Square src() const {
            return static_cast<Square>(code & SQUARE_BITS);
        }

        inline Square dst() const {
            return static_cast<Square>((code >> DST_SHIFT) & SQUARE_BITS);
        }

        inline MoveType moveType() const{
            return static_cast<MoveType>(MOVE_TYPE_BITS & code);
        }

        inline CastlingType castlingType() const{
            assert(moveType()==CASTLING_MOVE);
            return static_cast<CastlingType>(code >> EXTRA_CODE_SHIFT);
        }

        inline PieceType promotionPieceType() const{
            assert(moveType()==PROMOTION_MOVE);
            return fromPromotionCode(code >> EXTRA_CODE_SHIFT);
        }

        inline bool isEnPassantCapture() const{
            assert(moveType()==EN_PASSANT_MOVE);
            return code >> EXTRA_CODE_SHIFT;
        }

        inline bool isOk() const{
            return src() != dst();
        }

        // todo: change square to unsigned int
        static constexpr inline NewMove normalMove(Square src, Square dst) {
            return NewMove(src | (dst << DST_SHIFT));
        }

        static constexpr inline NewMove castle(CastlingType castlingType) {
            return castlingMoves[castlingType];
        }

        static constexpr inline NewMove promotionMove(Square src, Square dst, PieceType promotionPiece) {
            return NewMove(
                    src | (dst << DST_SHIFT) | PROMOTION_MOVE | (promotionCode(promotionPiece) << EXTRA_CODE_SHIFT));
        }

        static constexpr inline NewMove pawnForward2(Square src, Square dst) {
            return NewMove(src | (dst << DST_SHIFT) | EN_PASSANT_MOVE);
        }

        static constexpr inline NewMove enPassant(Square src, Square dst) {
            return NewMove(src | (dst << DST_SHIFT) | EN_PASSANT_MOVE | EN_PASSANT_CAPTURE);
        }

        static constexpr inline NewMove invalid(){
            return NewMove(0);
        }

        inline constexpr bool operator==(const NewMove &other) const {
            return code == other.code;
        }

        inline constexpr bool operator!=(const NewMove &other) const {
            return !(other == *this);
        }

        bool matchesMoveInput(MoveInputData moveInputData) const{
            //todo: add promotion
            return moveInputData.src == src() && moveInputData.dst == dst();
        }
    };

    ostream &operator<<(ostream &os, const NewMove &move);

    }


#endif //CHESS_NEWMOVE_H
