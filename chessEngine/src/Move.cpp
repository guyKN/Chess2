//
// Created by guykn on 12/9/2020.
//

#include "Move.h"
#include <iostream>
#include <string>
#include <Bitboards.h>

namespace Chess {
    Move Move::invalidMove(SQ_INVALID, SQ_INVALID, PIECE_INVALID, PIECE_INVALID);


    std::ostream& Move::longNotation(std::ostream &outputStream) const {
        if (!isOk()) {
            outputStream << "ILLEGAL";
        } else {
            if (castlingType != CASTLE_NONE) {
                outputStream << (isKingSide(castlingType) ? "o-o  " : "o-o-o");
            } else if (pieceTypeOf(srcPiece) != PIECE_TYPE_PAWN) {
                outputStream << toChar(pieceTypeOf(srcPiece))
                             << toString(srcSquare)
                             << ((dstPiece == PIECE_NONE) ? '-' : 'x')
                             << toString(dstSquare);
            } else {
                outputStream << Chess::toString(srcSquare)
                             << ((dstPiece == PIECE_NONE) ? '-' : 'x')
                             << toString(dstSquare)
                             << ' ';
            }
        }
        return outputStream;
    }

    bool Move::isOk() const {
        return square_ok(srcSquare) &&
               square_ok(dstSquare) &&
               srcSquare != dstSquare &&
               srcPiece != PIECE_NONE && pieceOk(srcPiece) &&
               pieceOk(dstPiece) &&
               (dstPiece == PIECE_NONE || ~playerOf(dstPiece) == playerOf(srcPiece)) &&
               (pieceOk(promotionPiece)) &&
               playerOf(promotionPiece) == playerOf(srcPiece) &&
               (promotionPiece == srcPiece || isValidPromotion(pieceTypeOf(promotionPiece)));
    }

    bool Move::operator==(const Move &other) const {
        return this->srcPiece == other.srcPiece &&
               this->dstPiece == other.dstPiece &&
               this->srcSquare == other.srcSquare &&
               this->dstSquare == other.dstSquare &&
               this->promotionPiece == other.promotionPiece &&
               this->castlingType == other.castlingType;
    }

    bool Move::matchesMoveInput(MoveInputData moveInputData) const {
        return moveInputData.src == srcSquare &&
               moveInputData.dst == dstSquare;
    }

    Move::Move(CastlingData castlingData) :
            srcSquare(castlingData.kingSrc),
            dstSquare(castlingData.kingDst),
            srcPiece(makePiece(PIECE_TYPE_KING, playerOf(castlingData.castlingType))),
            dstPiece(PIECE_NONE),
            promotionPiece(srcPiece),
            pawnForward2Square(SQ_INVALID),
            castlingType(castlingData.castlingType),
            isEnPassant(false){}

    const Move Move::castleMoves[NUM_CASTLE_TYPES] = {
            Move(CastlingData::fromCastlingType(CASTLE_WHITE_KING_SIDE)),
            Move(CastlingData::fromCastlingType(CASTLE_WHITE_QUEEN_SIDE)),
            Move(CastlingData::fromCastlingType(CASTLE_BLACK_KING_SIDE)),
            Move(CastlingData::fromCastlingType(CASTLE_BLACK_QUEEN_SIDE))
    };
}
