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
        if (moveString.length() != 4) {
            return invalidMove;
        }
        File fileSrc = parseFile(moveString[0]);
        Rank rankSrc = parseRank(moveString[1]);

        File fileDst = parseFile(moveString[2]);
        Rank rankDst = parseRank(moveString[3]);

        if (!file_ok(fileSrc) || !file_ok(fileDst) || !rank_ok(rankSrc) || !rank_ok(rankDst)) {
            return invalidMove;
        }
        return MoveInputData(makeSquare(rankSrc, fileSrc), makeSquare(rankDst, fileDst));
    }

}