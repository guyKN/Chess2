//
// Created by guykn on 1/17/2021.
//

#ifndef CHESS_EXCHANGE_EVALUATION_H
#define CHESS_EXCHANGE_EVALUATION_H

#include "types.h"

namespace Chess {

    extern Score staticExchangeEvalTable[NUM_THREAT_COMBINATIONS][NUM_NON_EMPTY_PIECES];
    //todo: replace score with char to make the array take less space

    inline Score staticExchangeEval(ThreatMap threatMap, Piece piece){
        assert(pieceOk(piece) && piece != PIECE_NONE && threatMap<=THREAT_MAP_ALL);
        return staticExchangeEvalTable[threatMap][piece];
    }

    void initExchangeMaps();
}
#endif //CHESS_EXCHANGE_EVALUATION_H
