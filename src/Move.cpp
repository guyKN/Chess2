//
// Created by guykn on 12/9/2020.
//

#include "Move.h"
#include <iostream>
#include <string>
#include <Bitboards.h>

namespace Chess {
    Move Move::invalidMove(SQ_INVALID, SQ_INVALID, PIECE_INVALID, PIECE_INVALID);


    std::ostream &operator<<(std::ostream &outputStream, Move &move) {
        if (!move.isOk()) {
            outputStream << "INVALID";
        } else {
            if (pieceTypeOf(move.srcPiece) != PIECE_TYPE_PAWN) {
                outputStream << toChar(pieceTypeOf(move.srcPiece))
                             << Chess::toString(move.srcSquare)
                             << ((move.dstPiece == PIECE_NONE) ? '-' : 'x')
                             << Chess::toString(move.dstSquare);
            } else {
                outputStream << Chess::toString(move.srcSquare)
                             << ((move.dstPiece == PIECE_NONE) ? '-' : 'x')
                             << Chess::toString(move.dstSquare)
                             << ' ';
            }
        }
        return outputStream;
    }

    bool Move::isOk() const {
        return square_ok(srcSquare) &&
               square_ok(dstSquare) &&
               srcSquare != dstSquare &&
               srcPiece != PIECE_NONE && piece_ok(srcPiece) &&
               (dstPiece == PIECE_NONE || ~playerOf(dstPiece) == playerOf(srcPiece));
    }

    bool Move::operator==(Move other) const {
        return this->srcPiece == other.srcPiece &&
               this->dstPiece == other.dstPiece &&
               this->srcSquare == other.srcSquare &&
               this->dstSquare == other.dstSquare;
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
            castlingType(castlingData.castlingType) {}

    const Move Move::castleMoves[NUM_CASTLE_TYPES] = {
            Move(CastlingData::fromCastlingType(CASTLE_WHITE_KING_SIDE)),
            Move(CastlingData::fromCastlingType(CASTLE_WHITE_QUEEN_SIDE)),
            Move(CastlingData::fromCastlingType(CASTLE_BLACK_KING_SIDE)),
            Move(CastlingData::fromCastlingType(CASTLE_BLACK_QUEEN_SIDE))
    };
}
