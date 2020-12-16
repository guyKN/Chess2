//
// Created by guykn on 12/11/2020.
//

#include <string>
#include "MoveInputData.h"
namespace Chess {
    const MoveInputData MoveInputData::invalidMove(SQ_INVALID, SQ_INVALID);
    MoveInputData::MoveInputData(const Chess::Square src, const Chess::Square dst) : src(src), dst(dst) {}

    Chess::MoveInputData Chess::MoveInputData::parse(string moveString) {
        moveString = removeSpaces(moveString);
        if (moveString.length() != 5 || moveString[2] != '-') {
            return invalidMove;
        }
        File fileSrc = parseFile(moveString[0]);
        Rank rankSrc = parseRank(moveString[1]);

        File fileDst = parseFile(moveString[3]);
        Rank rankDst = parseRank(moveString[4]);

        if (!file_ok(fileSrc) || !file_ok(fileDst) || !rank_ok(rankSrc) || !rank_ok(rankDst)) {
            return invalidMove;
        }
        return MoveInputData(makeSquare(rankSrc, fileSrc), makeSquare(rankDst, fileDst));
    }

    MoveInputData MoveInputData::readMove(std::istream &inputStream) {
        string str;
        std::getline(inputStream, str);
        return parse(str);
    }
}