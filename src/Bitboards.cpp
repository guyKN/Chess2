//
// Created by guykn on 12/12/2020.
//

#include "Bitboards.h"


namespace Chess {

    Bitboard randomBitboard() {
        Bitboard u1, u2, u3, u4;
        u1 = (Bitboard)(rand()) & 0xFFFF; u2 = (Bitboard)(rand()) & 0xFFFF;
        u3 = (Bitboard)(rand()) & 0xFFFF; u4 = (Bitboard)(rand()) & 0xFFFF;
        return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
    }
    Bitboard randomBitboard_fewBits(){
        return randomBitboard() & randomBitboard() & randomBitboard();
    }





    const Bitboard rankShiftMasks[5] = {~maskOfRank(RANK_1) & ~maskOfRank(RANK_2),
                                        ~maskOfRank(RANK_1),
                                        BITBOARD_FULL,
                                        ~maskOfRank(RANK_8),
                                        ~maskOfRank(RANK_8) & ~maskOfRank(RANK_7)};


    Bitboard knightMovesFrom_slow(Bitboard squareMask) {
        return shiftWithMask<1, 2>(squareMask) |
               shiftWithMask<1, -2>(squareMask) |
               shiftWithMask<-1, 2>(squareMask) |
               shiftWithMask<-1, -2>(squareMask) |
               shiftWithMask<2, 1>(squareMask) |
               shiftWithMask<2, -1>(squareMask) |
               shiftWithMask<-2, 1>(squareMask) |
               shiftWithMask<-2, -1>(squareMask);
    }

    Bitboard kingMovesFrom_slow(Bitboard squareMask) {
        return shiftWithMask<1, 1>(squareMask) |
               shiftWithMask<1, 0>(squareMask) |
               shiftWithMask<1, -1>(squareMask) |
               shiftWithMask<0, 1>(squareMask) |
               shiftWithMask<0, -1>(squareMask) |
               shiftWithMask<-1, 1>(squareMask) |
               shiftWithMask<-1, 0>(squareMask) |
               shiftWithMask<-1, -1>(squareMask);
    }


    template<int rankShift, int fileShift>
    Bitboard expandToEdge(Bitboard squareMask) {
        assert(squareMask_ok(static_cast<SquareMask>(squareMask)));
        constexpr int totalShift = fileShift + rankShift * NUM_FILES;
        Bitboard shiftMask = rankShiftMask<rankShift>() & fileShiftMask<fileShift>();
        Bitboard output = BITBOARD_EMPTY;

        squareMask &= shiftMask;
        while (squareMask) {
            squareMask = signedShift<totalShift>(squareMask);
            squareMask &= shiftMask;
            output |= squareMask;
        }
        return output;
    }

    template<int rankShift, int fileShift>
    Bitboard expandUntilCollision(Bitboard squareMask, Bitboard otherPieces) {
        assert(squareMask_ok(static_cast<SquareMask>(squareMask)));
        constexpr int totalShift = fileShift + rankShift * NUM_FILES;
        Bitboard notEdgesMask = rankShiftMask<rankShift>() & fileShiftMask<fileShift>();
        Bitboard shiftMask = notEdgesMask & ~otherPieces;

        Bitboard output = BITBOARD_EMPTY;

        squareMask &= shiftMask;
        while (squareMask) {
            squareMask = signedShift<totalShift>(squareMask);
            output |= squareMask;
            squareMask &= shiftMask;
        }
        return output;
    }

    Bitboard rookMovesFrom_slow(Square square, Bitboard otherPieces) {
        SquareMask squareMask = maskOf(square);
        return expandUntilCollision<1, 0>(squareMask, otherPieces) |
               expandUntilCollision<-1, 0>(squareMask, otherPieces) |
               expandUntilCollision<0, 1>(squareMask, otherPieces) |
               expandUntilCollision<0, -1>(squareMask, otherPieces);
    }

    Bitboard bishopMovesFrom_slow(Square square, Bitboard otherPieces) {
        SquareMask squareMask = maskOf(square);
        return expandUntilCollision<1, 1>(squareMask, otherPieces) |
               expandUntilCollision<-1, 1>(squareMask, otherPieces) |
               expandUntilCollision<1, -1>(squareMask, otherPieces) |
               expandUntilCollision<-1, -1>(squareMask, otherPieces);
    }

    Bitboard queenMovesFrom_slow(Square square, Bitboard otherPieces) {
        return bishopMovesFrom_slow(square, otherPieces) | rookMovesFrom_slow(square, otherPieces);
    }

    /// list of all squares that need to be hashed to calculate a rook movement
    Bitboard rookRelevantSquaresMask(Square square) {
        SquareMask squareMask = maskOf(square);

        return expandToEdge<1, 0>(squareMask) |
               expandToEdge<-1, 0>(squareMask) |
               expandToEdge<0, 1>(squareMask) |
               expandToEdge<0, -1>(squareMask);
    }

    Bitboard bishopRelevantSquaresMask(Square square) {
        SquareMask squareMask = maskOf(square);

        return expandToEdge<1, 1>(squareMask) |
               expandToEdge<1, -1>(squareMask) |
               expandToEdge<-1, 1>(squareMask) |
               expandToEdge<-1, -1>(squareMask);
    }

    ///for every 1 in the bitboard, output an array where that one turns into a 0 or a 1
    ///eg. 10010 turns into [00000,10000,00010,10010]
    ///the length of the output array must be equal to 2^(number of 1s in the Bitboard)
    void toggleBits(Bitboard bitboard, Bitboard *outputArray) {
        int population = populationCout(bitboard);
        cout << "population: " << population << "\n";
        unsigned int numCombinations = 1u << population;
        cout << "numCombinations: " << numCombinations << "\n";

        Bitboard individualSquares[population];
        Bitboard *ptr = &individualSquares[0];
        while (bitboard) {
            *(ptr++) = popLsbMask(&bitboard);
        }

        for (unsigned int i = 0; i < numCombinations; i++) {
            (*outputArray) = BITBOARD_EMPTY;
            for (int j = 0; j < population; j++) {
                if ((i >> j) & 1) {
                    (*outputArray) |= individualSquares[j];
                }
            }
            outputArray++;
        }
    }

    void magicHashAll(Bitboard hashFactor, Bitboard *inputArray, Bitboard *outputArray, unsigned int arrayLength) {
        for (int i = 0; i < arrayLength; i++, inputArray++, outputArray++) {
            *outputArray = *inputArray * hashFactor;
        }
    }

    int maxShiftThatKeepsDifference(Bitboard bitboard1, Bitboard bitboard2) {
        int shift = 63;
        while (shift > 0 && ((bitboard1 >> shift) == (bitboard2 >> shift))){
            shift--;
        }
        return shift;
    }

    int maxPossibleShift(Bitboard *movesArray, Bitboard *hashedArray, unsigned int arrayLength, int currentBest) {
        int maxPossibleShift =64;
        for (int i = 0; i < arrayLength; i++) {
            for (int j = i+1; j < arrayLength; j++) {
                if (movesArray[i] != movesArray[j]){
                    int currentMaxPossibleShift = maxShiftThatKeepsDifference(hashedArray[i], hashedArray[j]);
                    if (currentMaxPossibleShift < maxPossibleShift){
                        maxPossibleShift = currentMaxPossibleShift;
                    }
                }
            }
        }
        return maxPossibleShift;
    }

    //todo: allocate arrays on heap?
    void initBishopLookup(Square square) {
        Bitboard relevantSquares = bishopRelevantSquaresMask(square);
        int numRelevantSquares = populationCout(relevantSquares);
        unsigned int numPermutations = 1u << numRelevantSquares;
        Bitboard boardPositions[numPermutations];
        Bitboard legalMoves[numPermutations];
        Bitboard hashed[numPermutations];
        toggleBits(relevantSquares, &boardPositions[0]);
        for (int i = 0; i < numPermutations; ++i) {
            legalMoves[i] = bishopMovesFrom_slow(square, boardPositions[i]);
        }

        int overallBestShift = 0;
        int bestMagicHashFactor=0;
        for(int i=0;i<100000;i++){
            Bitboard magicHashFactor = randomBitboard_fewBits();
            magicHashAll(magicHashFactor, legalMoves, hashed, numPermutations);
            int bestShift = maxPossibleShift(legalMoves, hashed, numPermutations, overallBestShift);
            if(bestShift > overallBestShift){
                overallBestShift = bestShift;
                bestMagicHashFactor = magicHashFactor;
            }
        }
        cout << "best shift:" << overallBestShift << "\n";
        printBitboard(bestMagicHashFactor);
    }


    Bitboard knightMovesLookup[NUM_SQUARES];
    Bitboard kingMovesLookup[NUM_SQUARES];

    void initKingAndKnight() {
        SquareMask squareMask = SQUARE_MASK_FIRST;
        for (Square square = SQ_FIRST; square <= SQ_LAST; ++square, squareMask <<= 1) {
            knightMovesLookup[square] = knightMovesFrom_slow(squareMask);
            kingMovesLookup[square] = kingMovesFrom_slow(squareMask);
        }
    }

    bool lookUpTablesReady = false;

    void initLookupTables() {
        if (!lookUpTablesReady) {
            initBishopLookup(SQ_E3);
            initKingAndKnight();
            lookUpTablesReady = true;
        }
    }


    constexpr Bitboard MASK_RANK_1 = 0x00000000000000FF;
    const Bitboard rankMasks[NUM_RANKS] = {MASK_RANK_1, MASK_RANK_1 << 8, MASK_RANK_1 << 16, MASK_RANK_1 << 24,
                                           MASK_RANK_1 << 32, MASK_RANK_1 << 40, MASK_RANK_1 << 48,
                                           MASK_RANK_1 << 56};


    static constexpr Bitboard MASK_FILE_A = 0x0101010101010101;
    const Bitboard fileMasks[NUM_FILES] = {MASK_FILE_A, MASK_FILE_A << 1, MASK_FILE_A << 2, MASK_FILE_A << 3,
                                           MASK_FILE_A << 4,
                                           MASK_FILE_A << 5, MASK_FILE_A << 6, MASK_FILE_A << 7};

    const Bitboard fileShiftMasks[5] = {~maskOfFile(FILE_A) & ~maskOfFile(FILE_B),
                                        ~maskOfFile(FILE_A),
                                        BITBOARD_FULL,
                                        ~maskOfFile(FILE_H),
                                        ~maskOfFile(FILE_H) & ~maskOfFile(FILE_G)};


}

