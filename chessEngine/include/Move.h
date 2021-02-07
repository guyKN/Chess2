//
// Created by guykn on 1/9/2021.
//

#ifndef CHESS_MOVE_H
#define CHESS_MOVE_H

#include <ostream>
#include "types.h"
#include "Bitboards.h"
#include "MoveInputData.h"

namespace Chess {

    class Move {
        //todo: check the best order for bits. the last and first bits are importantlyly convenient, because they only require a shift or mask operation, not both
        /// uses 32 bits
        /// the first 16 bits represents the move itself. Moves may be stored in transposition table using just the first 16 bits
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
        /// if dst==0, and src==0, then the bestMove_ is invalid
        /// the last 16 bits are a score given to the move for move ordering.
        /// Since comparasion functions the later bits first, it means we can easly sort by move value without having to mask or anything
        /// invalid move is represnted as all zeroes.
        /// a null move is represented as a move from h8 to h8, the square bits are 1, all the other bits are 0.

    public:
        using CodeType = uint32_t;

        constexpr explicit Move(CodeType code) : code(code) {}

    private:
        CodeType code;
        static constexpr int DST_SHIFT = 6;
        static constexpr int MOVE_TYPE_SHIFT = 12;
        static constexpr int EXTRA_CODE_SHIFT = 14;
        static constexpr int MOVE_SCORE_SHIFT = 16;

    public:
        enum MoveType : CodeType {
            NORMAL_MOVE = 0b00 << MOVE_TYPE_SHIFT,
            PROMOTION_MOVE = 0b01 << MOVE_TYPE_SHIFT,
            CASTLING_MOVE = 0b10 << MOVE_TYPE_SHIFT,
            EN_PASSANT_MOVE = 0b11 << MOVE_TYPE_SHIFT
        };
    private:

        static constexpr CodeType SQUARE_BITS = 0b111111;
        static constexpr CodeType MOVE_TYPE_BITS = 0b11 << MOVE_TYPE_SHIFT;
        static constexpr CodeType EN_PASSANT_CAPTURE = 1 << EXTRA_CODE_SHIFT;
        static constexpr CodeType EXTRA_CODE_BITS = 0b11 << EXTRA_CODE_SHIFT;
        static constexpr CodeType MOVE_BITS = 0xFFFF;

        static Move castle_slow(CastlingType castlingType);

        static const Move castlingMoves[NUM_CASTLE_TYPES];

        static constexpr inline CodeType promotionCode(PieceType pieceType) {
            assert(isValidPromotion(pieceType));
            return pieceType - 1;
        }

        static constexpr inline PieceType fromPromotionCode(CodeType promotionCode) {
            return static_cast<PieceType>(promotionCode + 1);
        }


    public:
        inline int getCode() const {
            return code;
        }

        inline Square src() const {
            return static_cast<Square>(code & SQUARE_BITS);
        }

        inline Square dst() const {
            return static_cast<Square>((code >> DST_SHIFT) & SQUARE_BITS);
        }

        inline MoveType moveType() const {
            return static_cast<MoveType>(MOVE_TYPE_BITS & code);
        }

        inline CastlingType castlingType() const {
            assert(moveType() == CASTLING_MOVE);
            return static_cast<CastlingType>((code & EXTRA_CODE_BITS) >> EXTRA_CODE_SHIFT);
        }

        inline PieceType promotionPieceType() const {
            assert(moveType() == PROMOTION_MOVE);
            return fromPromotionCode((code & EXTRA_CODE_BITS) >> EXTRA_CODE_SHIFT);
        }

        inline bool isEnPassantCapture() const {
            assert(moveType() == EN_PASSANT_MOVE);
            return (code & EXTRA_CODE_BITS);
        }

        inline StaticEvalScore score() const{
            return static_cast<StaticEvalScore>(code >> MOVE_SCORE_SHIFT);
        }

        inline bool isOk() const {
            return src() != dst();
        }

        Move() : code(0) {}

        static constexpr inline Move normalMove(Square src, Square dst, unsigned score) {
            return Move(src | (dst << DST_SHIFT) | (score << MOVE_SCORE_SHIFT));
        }

        static constexpr inline Move normalMove(Square src, Square dst) {
            return normalMove(src, dst, 0);
        }

        static inline Move castle(CastlingType castlingType) {
            return castlingMoves[castlingType];
        }

        static constexpr inline Move promotionMove(Square src, Square dst, PieceType promotionPiece, unsigned score) {
            return Move(
                    src | (dst << DST_SHIFT) | PROMOTION_MOVE | (promotionCode(promotionPiece) << EXTRA_CODE_SHIFT) |
                    (score << MOVE_SCORE_SHIFT));
        }

        static constexpr inline Move promotionMove(Square src, Square dst, PieceType promotionPiece) {
            return promotionMove(src, dst, promotionPiece, 0);
        }

        static constexpr inline Move pawnForward2(Square src, Square dst, unsigned score) {
            return Move(src | (dst << DST_SHIFT) | EN_PASSANT_MOVE | (score << MOVE_SCORE_SHIFT));
        }

        static constexpr inline Move pawnForward2(Square src, Square dst) {
            return pawnForward2(src, dst, 0);
        }

        static constexpr inline Move enPassantCapture(Square src, Square dst, unsigned score) {
            return Move(src | (dst << DST_SHIFT) | EN_PASSANT_MOVE | EN_PASSANT_CAPTURE | (score << MOVE_SCORE_SHIFT));
        }

        static constexpr inline Move enPassantCapture(Square src, Square dst) {
            return enPassantCapture(src, dst, 0);
        }

        static constexpr inline Move invalid() {
            return Move(0);
        }

        static constexpr inline Move nullMove(){
            return Move::normalMove(SQ_H8, SQ_H8);
        }

        inline constexpr operator bool() const{
            return code;
        }

        inline bool isSameAs(const Move& other) const{
            return (this->code & MOVE_BITS) == (other.code & MOVE_BITS);
        }

        inline constexpr bool operator==(const Move &other) const {
            return code == other.code;
        }

        inline constexpr bool operator!=(const Move &other) const {
            return !(other == *this);
        }

        inline constexpr bool operator>(const Move &other) const {
            return code > other.code;
        }

        inline constexpr bool operator<=(const Move &other) const {
            return !(*this>other);
        }

        inline constexpr bool operator<(const Move &other) const {
            return code < other.code;
        }

        inline constexpr bool operator>=(const Move &other) const {
            return !(*this<other);
        }

        bool matchesMoveInput(MoveInputData moveInputData) const;
    };

    ostream &operator<<(ostream &os, const Move &move);

}


#endif //CHESS_MOVE_H
