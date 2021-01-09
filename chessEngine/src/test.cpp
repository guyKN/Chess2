//
// Created by guykn on 1/4/2021.
//
#include <types.h>
#include <Bitboards.h>
using std::cout;
namespace Chess {
    void runTest() {
        Bitboard otherPieces = 0xadef20301014eea9;
        otherPieces &= notSquareMask(SQ_C3);
        MagicHasData &magicHasData = bishopData[SQ_B2].magicHashData;
        printBitboard(magicHasData.calculateSlidingMoves(otherPieces));
        cout << "\n";
    }
}