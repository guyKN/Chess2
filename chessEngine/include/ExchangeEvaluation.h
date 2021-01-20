//
// Created by guykn on 1/17/2021.
//

#ifndef CHESS_EXCHANGE_EVALUATION_H
#define CHESS_EXCHANGE_EVALUATION_H

#include "types.h"

namespace Chess {


    //todo: think about whether or not to include negetive scores, since when we probe these tables, it's after manually doing one capture
    extern StaticEvalScore staticExchangeEvalTable[NUM_NON_EMPTY_PIECES][NUM_THREAT_COMBINATIONS];

    inline StaticEvalScore staticExchangeEval(ThreatMap threatMap, Piece piece){
        assert(pieceOk(piece) && piece != PIECE_NONE && threatMap<=THREAT_MAP_ALL);
        return staticExchangeEvalTable[threatMap][piece];
    }

    void initExchangeLookup();
}
#endif //CHESS_EXCHANGE_EVALUATION_H
