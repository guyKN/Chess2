//
// Created by guykn on 1/17/2021.
//

#include <ExchangeEvaluation.h>
#include <iostream>
#include <Bitboards.h>
#include "types.h"

namespace Chess {

    Score staticExchangeEvalTable[NUM_THREAT_COMBINATIONS][NUM_NON_EMPTY_PIECES]{};


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


    Score basicPieceValue(PieceType pieceType) {
        static int basicPieceValues[NUM_PIECE_TYPES_NOT_EMPTY] = {100, 300, 300, 500, 900, 10000};
        return static_cast<Score>(basicPieceValues[pieceType]);
    }

    Score captureEval(ThreatMap threatMap, Piece occupiedPiece) {
        Player opponent = playerOf(occupiedPiece);
        Player currentPlayer = ~opponent;
        if (!(threatMap & threatMapOf(currentPlayer))) {
            return SCORE_ZERO;
        }
        Score captureScore = basicPieceValue(pieceTypeOf(occupiedPiece));
        Piece capturingPiece = popLsb(threatMap, currentPlayer);

        Score enemyCaptureScore = captureEval(threatMap, capturingPiece);

        if (enemyCaptureScore > 0) {
            return captureScore - enemyCaptureScore;
        } else {
            return captureScore;
        }
    }

    void initExchangeMaps() {
        for (ThreatMap threatMap = THREAT_MAP_NONE; threatMap <= THREAT_MAP_ALL; ++threatMap) {
            for (Piece piece = PIECE_FIRST; piece <= PIECE_LAST_NOT_EMPTY; ++piece) {
                staticExchangeEvalTable[threatMap][piece] = captureEval(threatMap, piece);
            }
        }
    }

    void debugExchangeMaps() {
        PRNG prng{1234567890};

        for (ThreatMap threatMap = THREAT_MAP_NONE; threatMap <= THREAT_MAP_ALL; ++threatMap) {
            for (Piece piece = PIECE_FIRST; piece <= PIECE_LAST_NOT_EMPTY; ++piece) {
                if (!(prng.rand<Bitboard>() & 0b11111)) {
                    std::cout << "occupancy: " << piece << "\n" <<
                              "threats: " << threatMap << "\n" <<
                              "see: " << staticExchangeEvalTable[threatMap][piece] << "\n" <<
                              "+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
                }
            }
        }
    }
}
