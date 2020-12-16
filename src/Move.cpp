//
// Created by guykn on 12/9/2020.
//

#include "Move.h"
#include <iostream>
#include <string>

using namespace Chess;

Move Move::invalidMove(SQ_INVALID, SQ_INVALID, PIECE_INVALID, PIECE_FIRST_BLACK);

Move::Move(const Square src, const Square dst, const Piece srcPiece, const Piece dstPiece) :
        srcSquare(src), dstSquare(dst), srcPiece(srcPiece), dstPiece(dstPiece) {}

std::ostream &Chess::operator<<(std::ostream &outputStream, Move &move) {
    if (!move.isOk()) {
        outputStream << "INVALID";
    } else {
        outputStream << Chess::toString(move.srcSquare) << '-'
                     << Chess::toString(move.dstSquare);
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
