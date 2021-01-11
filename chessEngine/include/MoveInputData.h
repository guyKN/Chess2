//
// Created by guykn on 12/11/2020.
//

#ifndef CHESS_MOVEINPUTDATA_H
#define CHESS_MOVEINPUTDATA_H

#include "types.h"


using std::string;

namespace Chess {
    struct MoveInputData {
        static const MoveInputData invalidMove;
        Square src;
        Square dst;
        PieceType promotionPiece; //promotion piece may be PIECE_TYPE_NONE, in which case there is not promotion
        bool isKingSideCastle;
        bool isQueenSideCastle;

        bool isOk() const {
            return square_ok(src) && square_ok(dst) && src != dst;
        }

        static MoveInputData parse(const std::string& str);
    };

    template<class outputStream>
    inline outputStream &operator>>(outputStream &stream, MoveInputData &moveInputData){
        string str;
        stream >> str;
        moveInputData = MoveInputData::parse(str);
        return stream;
    }



}

#endif //CHESS_MOVEINPUTDATA_H