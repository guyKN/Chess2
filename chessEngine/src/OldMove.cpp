//
// Created by guykn on 12/9/2020.
//

#include "OldMove.h"
#include <iostream>
#include <string>
#include <Bitboards.h>

namespace Chess {
    OldMove OldMove::invalidMove(SQ_INVALID, SQ_INVALID, PIECE_INVALID, PIECE_INVALID);

    std::ostream& OldMove::longNotation(std::ostream &outputStream) const {
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

    bool OldMove::isOk() const {
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

    bool OldMove::operator==(const OldMove &other) const {
        return this->srcPiece == other.srcPiece &&
               this->dstPiece == other.dstPiece &&
               this->srcSquare == other.srcSquare &&
               this->dstSquare == other.dstSquare &&
               this->promotionPiece == other.promotionPiece &&
               this->castlingType == other.castlingType;
    }

    bool OldMove::matchesMoveInput(MoveInputData moveInputData) const {
        return moveInputData.src == srcSquare &&
               moveInputData.dst == dstSquare;
    }

    OldMove::OldMove(const CastlingData &castlingData) :
            srcSquare(castlingData.kingSrc),
            dstSquare(castlingData.kingDst),
            srcPiece(makePiece(PIECE_TYPE_KING, playerOf(castlingData.castlingType))),
            dstPiece(PIECE_NONE),
            promotionPiece(srcPiece),
            pawnForward2Square(SQ_INVALID),
            castlingType(castlingData.castlingType),
            isEnPassant(false){}

    const OldMove OldMove::castleMoves[NUM_CASTLE_TYPES] = {
            OldMove(CastlingData::fromCastlingType(CASTLE_WHITE_KING_SIDE)),
            OldMove(CastlingData::fromCastlingType(CASTLE_WHITE_QUEEN_SIDE)),
            OldMove(CastlingData::fromCastlingType(CASTLE_BLACK_KING_SIDE)),
            OldMove(CastlingData::fromCastlingType(CASTLE_BLACK_QUEEN_SIDE))
    };
}
