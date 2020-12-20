//
// Created by guykn on 12/12/2020.
//

#include "Bitboards.h"


namespace Chess {

    static const int NUM_ATTEMPTS_SEARCH_MAGIC_HASH = 100000000;


    Bitboard bishopSeeds[NUM_SQUARES] = {0x3004105082045010, 0x4016100600a01080, 0x4010018200411000, 0x4040080200080,
                                         0x304050400006500, 0x411010590010100, 0x82820cc21202012, 0x21405208200200,
                                         0x40481010261040,
                                         0xf0010402808186, 0x4000080081a20200, 0x88c0408100020, 0x204104c0900a4,
                                         0x4301123010080400, 0x810008904821806, 0xa0041345044, 0x810422042500104,
                                         0x810304810410440, 0x808144110040,
                                         0x8105002840123220, 0x200010190a020, 0x4102000100708441, 0x40052020a1200,
                                         0x200100404c02210c, 0x4209040080a0812, 0x2244442001084800, 0x40112e0004024008,
                                         0x8202202002403006, 0x20412081c042000, 0x909044800802000, 0x41400408080b0,
                                         0x720810100640210, 0x408900800042050, 0xa00962021081800, 0x640a8810008a03,
                                         0x500400804400, 0x2d02060102180, 0x4081082501010, 0x8001060892640400,
                                         0x401004022026a, 0x8a008443002008, 0x1040120000c80, 0x9208e2802020118,
                                         0x10014010400201, 0x30e20084002080, 0x1002220942001404, 0x3090240087800c00,
                                         0x1083182481000084, 0x6080412080044, 0x41020184045a6000, 0x804001202900000,
                                         0x20020880c00, 0x1616204a0820041, 0x400c21041960000, 0x2080600860000,
                                         0x504881204002204, 0x82084a00800, 0x8ea05602012c0a20, 0x802008a1080800,
                                         0x2400008208804, 0x201c0040010c40, 0x804c400c84080202, 0x8931041080080,
                                         0x2a00050860090};
    Bitboard rookSeeds[NUM_SQUARES];

    int bishopSizeScore = 0;

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

    Bitboard knightMovesLookup[NUM_SQUARES];
    Bitboard kingMovesLookup[NUM_SQUARES];

    MagicHasData bishopHashData[NUM_SQUARES] = {};
    MagicHasData rookHashData[NUM_SQUARES] = {};
    Bitboard rookBishopMoveTable[1000000] = {};

    Bitboard *currentBishopRookLookupPointer = &rookBishopMoveTable[0];


    /* Seed */
    std::random_device rd;

    /* Random number generator */
    std::default_random_engine generator(rd());

    /* Distribution on which to apply the generator */
    std::uniform_int_distribution<long long unsigned> distribution(BITBOARD_EMPTY,BITBOARD_FULL);

    Bitboard randomBitboard() {
        return distribution(generator);
    }

    Bitboard randomBitboard_fewBits() {
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

    Bitboard bishopMovesFrom_slow(Square square, Bitboard blockers) {
        SquareMask squareMask = maskOf(square);
        return expandUntilCollision<1, 1>(squareMask, blockers) |
               expandUntilCollision<-1, 1>(squareMask, blockers) |
               expandUntilCollision<1, -1>(squareMask, blockers) |
               expandUntilCollision<-1, -1>(squareMask, blockers);
    }

    Bitboard queenMovesFrom_slow(Square square, Bitboard otherPieces) {
        return bishopMovesFrom_slow(square, otherPieces) | rookMovesFrom_slow(square, otherPieces);
    }

    /// list of all squares that need to be hashed to calculate a rook movement
    Bitboard rookBlockersMask(Square square) {
        SquareMask squareMask = maskOf(square);

        return expandToEdge<1, 0>(squareMask) |
               expandToEdge<-1, 0>(squareMask) |
               expandToEdge<0, 1>(squareMask) |
               expandToEdge<0, -1>(squareMask);
    }

    Bitboard bishopBlockersMask(Square square) {
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
        unsigned int numCombinations = 1u << population;
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
        while (shift > 0 && ((bitboard1 >> shift) == (bitboard2 >> shift))) {
            shift--;
        }
        return shift;
    }

    int maxPossibleShift(Bitboard *movesArray, Bitboard *hashedArray, unsigned int arrayLength, int previousBest) {
        int currentMaxShift = 63;
        for (int i = 0; i < arrayLength; i++) {
            for (int j = 0; j < arrayLength; j++) {
                if (movesArray[i] != movesArray[j]) {
                    int maxShift = maxShiftThatKeepsDifference(hashedArray[i], hashedArray[j]);
                    assert((hashedArray[i] >> maxShift) != (hashedArray[j] >> maxShift) ||
                           (maxShift == 0));
                    if (maxShift < currentMaxShift) {
                        currentMaxShift = maxShift;
                        if (currentMaxShift <= previousBest) {
                            return previousBest - 1;
                        }
                    }
                }
            }
        }
        return currentMaxShift;
    }

    Bitboard bruteForceSearchMagicHashFactor(unsigned int numPermutations,
                                             Bitboard *blockers,
                                             Bitboard *legalMoves,
                                             Bitboard *hashed,
                                             int numAttempts) {
        int overallBestShift = 0;
        Bitboard bestMagicHashFactor = 0;
        for (int i = 0; i < numAttempts; i++) {
            Bitboard magicHashFactor = randomBitboard_fewBits();
            magicHashAll(magicHashFactor, blockers, hashed, numPermutations);
            int bestShift = maxPossibleShift(legalMoves, hashed, numPermutations, overallBestShift);
            if (bestShift > overallBestShift) {
                overallBestShift = bestShift;
                bestMagicHashFactor = magicHashFactor;
            }
        }
        return bestMagicHashFactor;
    }


    void initBishopLookup(Square square, bool useSeeds) {
        Bitboard blockersMask = bishopBlockersMask(square);
        int population = populationCout(blockersMask);
        unsigned int numPermutations = 1u << population;
        Bitboard blockers[numPermutations];
        Bitboard legalMoves[numPermutations];
        Bitboard hashed[numPermutations];
        toggleBits(blockersMask, &blockers[0]);
        for (int i = 0; i < numPermutations; ++i) {
            legalMoves[i] = bishopMovesFrom_slow(square, blockers[i]);
        }
        Bitboard magicHashFactor;
        if (!useSeeds) {
            magicHashFactor = bruteForceSearchMagicHashFactor(numPermutations,
                                                              blockers,
                                                              legalMoves,
                                                              hashed,
                                                              NUM_ATTEMPTS_SEARCH_MAGIC_HASH);
        } else {
            magicHashFactor = bishopSeeds[square];
        }


        magicHashAll(magicHashFactor, blockers, hashed, numPermutations);
        int shift = maxPossibleShift(legalMoves, hashed, numPermutations, 0);

        int maxIndex = 0;
        for (int i = 0; i < numPermutations; ++i) {
            hashed[i] >>= shift;
            Bitboard &moves = currentBishopRookLookupPointer[hashed[i]];
            moves = legalMoves[i];
            if (hashed[i] > maxIndex) {
                maxIndex = hashed[i];
            }
        }

        MagicHasData magicHasData;
        magicHasData.mask = blockersMask;
        magicHasData.shift = shift;
        magicHasData.magicHashFactor = magicHashFactor;
        magicHasData.moveLookup = currentBishopRookLookupPointer;

        bishopHashData[square] = magicHasData;
        currentBishopRookLookupPointer += maxIndex;

        if (!useSeeds) {
            bishopSeeds[square] = magicHashFactor;
            bishopSizeScore += 64 - shift;
        }
    }

    void printSeeds() {
        cout << "Bishop Seeds: ";
        printArray(bishopSeeds, NUM_SQUARES) << "\n\n";
        cout << "Bishop Score: " << std::dec << bishopSizeScore << "\n";
    }


    void initLookupTables() {
        SquareMask squareMask = SQUARE_MASK_FIRST;
        for (Square square = SQ_FIRST; square <= SQ_LAST; ++square, squareMask <<= 1) {
            knightMovesLookup[square] = knightMovesFrom_slow(squareMask);
            kingMovesLookup[square] = kingMovesFrom_slow(squareMask);
#ifdef USE_SEEDS
            initBishopLookup(square, true);
#elif defined(GENERATE_SEEDS)
            initBishopLookup(square, false);
#else
#error must either use seeds or generate seeds
#endif
        }

#ifdef GENERATE_SEEDS
        printSeeds();
#endif
    }


    bool lookUpTablesReady = false;

    void initBitboards() {
        if (!lookUpTablesReady) {
            initLookupTables();
            lookUpTablesReady = true;
            //assert(false);
        }
    }


}

