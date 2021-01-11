//
// Created by guykn on 12/11/2020.
//

#include <string>
#include "MoveInputData.h"
#include "Uci.h"
namespace Chess {
    const MoveInputData MoveInputData::invalidMove{SQ_INVALID, SQ_INVALID};

    MoveInputData MoveInputData::parse(const std::string& str) {
        if (str == "o-o") {
            return {
                    SQ_FIRST,
                    SQ_FIRST,
                    PIECE_TYPE_NONE,
                    true,
                    false
            };
        } else if (str == "o-o-o") {
            return {
                    SQ_FIRST,
                    SQ_FIRST,
                    PIECE_TYPE_NONE,
                    false,
                    true
            };
        } else if (str.length() == 4 || str.length() == 5) {
            File srcFile = parseFile(str[0]);
            Rank srcRank = parseRank(str[1]);

            File dstFile = parseFile(str[2]);
            Rank dstRank = parseRank(str[3]);
            PieceType promotionPiece = str.length() == 5 ? parsePieceType(str[4]) : PIECE_TYPE_NONE;

            if (file_ok(srcFile) && rank_ok(dstRank) && file_ok(dstFile) && rank_ok(dstRank) &&
                (isValidPromotion(promotionPiece) || (promotionPiece == PIECE_TYPE_NONE))) {
                return{
                        makeSquare(srcRank, srcFile),
                        makeSquare(dstRank, dstFile),
                        promotionPiece,
                        false,
                        false
                };
            } else{
                return MoveInputData::invalidMove;
            }


        } else{
            return MoveInputData::invalidMove;
        }
    }
}