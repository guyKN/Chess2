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
        const Chess::Square src;
        const Chess::Square dst;

        MoveInputData(const Square src, const Square dst);

        bool isOk() const{
            return square_ok(src) && square_ok(dst);
        }

        static MoveInputData readMove(std::istream &inputStream = std::cin);

        static MoveInputData parse(string);
    };
}

#endif //CHESS_MOVEINPUTDATA_H