//
// Created by guykn on 1/17/2021.
//

#include <ExchangeEvaluation.h>
#include <iostream>
#include <Bitboards.h>
#include "types.h"

namespace Chess {

    StaticEvalScore staticExchangeEvalTable[NUM_NON_EMPTY_PIECES][NUM_THREAT_COMBINATIONS]{};


    std::ostream &operator<<(std::ostream &outputStream, ThreatMap threatMap) {

        while (threatMap & threatMapOf(WHITE)) {
            Piece piece = popLsb(threatMap, WHITE);
            outputStream << toChar2(pieceTypeOf(piece));
        }
        outputStream << "V";

        while (threatMap & threatMapOf(BLACK)) {
            Piece piece = popLsb(threatMap, BLACK);
            outputStream << toChar2(pieceTypeOf(piece));
        }

        return outputStream;
    }

    StaticEvalScore captureEval(ThreatMap threatMap, Piece occupiedPiece) {
        Player opponent = playerOf(occupiedPiece);
        Player currentPlayer = ~opponent;
        if (!(threatMap & threatMapOf(currentPlayer))) {
            return STATIC_SCORE_ZERO;
        }
        StaticEvalScore captureScore = staticPieceValue(occupiedPiece);
        Piece capturingPiece = popLsb(threatMap, currentPlayer);

        StaticEvalScore enemyCaptureScore = captureEval(threatMap, capturingPiece);

        if (enemyCaptureScore > 0) {
            return captureScore - enemyCaptureScore;
        } else {
            return captureScore;
        }
    }

    void initExchangeLookup() {
        for (Piece piece = PIECE_FIRST; piece <= PIECE_LAST_NOT_EMPTY; ++piece) {
            for (ThreatMap threatMap = THREAT_MAP_NONE; threatMap <= THREAT_MAP_ALL; ++threatMap) {
                staticExchangeEvalTable[piece][threatMap] = captureEval(threatMap, piece);
            }
        }
    }

    void debugExchangeLookup() {
        PRNG prng{12345890};
        for (ThreatMap threatMap = THREAT_MAP_NONE; threatMap <= THREAT_MAP_ALL; ++threatMap) {
            for (Piece piece = PIECE_FIRST; piece <= PIECE_LAST_NOT_EMPTY; ++piece) {
                if (!(prng.rand<Bitboard>() & 0b11111)) {
                    std::cout << "occupancy: " << piece << "\n" <<
                              "threatData: " << threatMap << "\n" <<
                              "see: " << staticExchangeEvalTable[threatMap][piece] << "\n" <<
                              "+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
                }
            }
        }
    }
}
