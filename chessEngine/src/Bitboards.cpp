//
// Created by guykn on 12/12/2020.
//

#include <ChessBoard.h>
#include <TransPositionTable.h>
#include "Bitboards.h"
#include "EvalData.h"


using std::cout;
namespace Chess {
    //todo: is using templates for this actualy a good idea?
    template<int rankShift, int fileShift, bool includeEdge>
    Bitboard expandToEdge(Bitboard squareMask);


    template<int rankShift, int fileShift>
    inline Bitboard expandToEdgeExclusive(Bitboard squareMask) {
        return expandToEdge<rankShift, fileShift, false>(squareMask);
    }

    template<int rankShift, int fileShift>
    inline Bitboard expandToEdgeInclusive(Bitboard squareMask) {
        return expandToEdge<rankShift, fileShift, true>(squareMask);
    }

    template<int rankShift, int fileShift>
    Bitboard expandUntilCollision(Bitboard squareMask, Bitboard otherPieces);

    static constexpr int LOOKUP_TABLE_LENGTH = 109'654;

    Bitboard bishopSeeds[NUM_SQUARES] = {0x5230100681840d46, 0xb4964c0808a50114, 0x2408908408841c80, 0x8808410904061f,
                                         0x2106021004c0010a, 0xca41882008047912, 0x7804225490082206, 0x96a9208410111060,
                                         0x100920385051a583, 0x4401f01081010822, 0x1601101911e10505, 0xa009141c0a800901,
                                         0x10be460610258502, 0x4403050328c02242, 0x9280942403384821, 0x21082086108220d0,
                                         0x90e424090041500, 0x1424080224042418, 0x2278049008812108, 0x1044000824001009,
                                         0x3808405a00008, 0x43a006103031530, 0x5204000282080258, 0x408302284040a18,
                                         0x81621084c8101060, 0x24d02608301a3e05, 0x60080c0018004690, 0x44148c0028012020,
                                         0x8004082014002009, 0x5406028108081111, 0x81404001186139e, 0x4a5600208a031128,
                                         0xc148470202001, 0xc540420005e2e40, 0x831242280250004e, 0x4c092008020901d0,
                                         0x28a1100400108120, 0x26c010200215800, 0xa080115005c01, 0x80904002a270110,
                                         0x85241b04400b5064, 0xa000480c9048e420, 0xa1114030003809, 0x208245a018080100,
                                         0xb040500200800817, 0x854100201720200, 0x80c809640b820400, 0x8618480785620085,
                                         0xe17105283240b1ce, 0x8603041202420000, 0x28344b2928080000, 0x8000d03842120025,
                                         0x24440611220a00e5, 0x8904500210091210, 0xb230909d1d140208, 0x20d04212040030a1,
                                         0x234805108034008, 0xc08c1905082a1200, 0x1002102e69080800, 0xde2600561e841104,
                                         0x71180c00c0104b15, 0x1012c04430122200, 0x31732060124c8314,
                                         0x4020080ba100c601};

    Bitboard rookSeeds[NUM_SQUARES] = {0x2080014000822690, 0x9200204100308200, 0x20008408200d120, 0x8100800a0100208,
                                       0x80260800040080, 0x28802400800b0600, 0x31000c2781000200, 0xc9000048a1001182,
                                       0x100800c40042087, 0xc43a004201610388, 0x10c1002003c50010, 0x4422000840106200,
                                       0x213002801003410, 0xc0a200190a007004, 0x9a04002b16541830, 0x9116001844060099,
                                       0x86008280004000a4, 0x2d48808020004000,
                                       0x280350020050040, 0x158a2001a0190c0, 0x100c028008000480, 0x4001010008140002,
                                       0x64140028258a10, 0x8840460000825c09, 0x180188208009c001, 0x99001c040042001,
                                       0x200e002200118340, 0xc212016a00102140, 0x1c00080080240080, 0x4250200804c0080,
                                       0xa00022400500837, 0x20c1085e00010a84, 0x1023c000800192, 0x305e0082a2004300,
                                       0x4840609202004080, 0x90014400c01800, 0xa780500501001801, 0x5c8280c200800c00,
                                       0x820e81214008510, 0x1402108a5a000114, 0xc1834008e98000, 0xc38200850004001,
                                       0x1061002001450011, 0x8810010060890010, 0x830d00080251000c, 0x2703000400430008,
                                       0x3ad0080116040030, 0x4601044100920004, 0x26010180412a0a00, 0x8200400102208100,
                                       0x480d406203847200, 0x40838a0060401200, 0x9841003800302d00, 0x6066420080040080,
                                       0xb800810321b7400, 0x41010c5124008200, 0x250800740610433, 0x8098850250604001,
                                       0x289712ca2000c101, 0x56e8209c10010009, 0x6826002830646056, 0x10720008900b0406,
                                       0x6844591000980604, 0x8c18464400811466};


    const Bitboard fileShiftMasks[5] = {~maskOf(FILE_A) & ~maskOf(FILE_B),
                                        ~maskOf(FILE_A),
                                        BITBOARD_FULL,
                                        ~maskOf(FILE_H),
                                        ~maskOf(FILE_H) & ~maskOf(FILE_G)};

    Bitboard knightMovesLookup[NUM_SQUARES];
    Bitboard kingMovesLookup[NUM_SQUARES];

    SlidingPieceData bishopData[NUM_SQUARES] = {};
    SlidingPieceData rookData[NUM_SQUARES] = {};
    Bitboard rookBishopMoveTable[LOOKUP_TABLE_LENGTH] = {};

    Bitboard *currentBishopRookLookupPointer = &rookBishopMoveTable[0];
    int currentLookupTableLength = 0;


    //todo: check if inline is faster
    void CastlingData::updateCastlingRights(Bitboard moveSquares, CastlingRights &castlingRightsMask) {
        for (CastlingType castlingType = CASTLE_FIRST; castlingType <= CASTLE_LAST; ++castlingType) {
            const CastlingData &castlingData = fromCastlingType(castlingType);
            // todo: is a single ? : operation faster than an if
            if (castlingData.moveDisablesCastling(moveSquares)) {
                castlingRightsMask &= ~castlingRightsOf(castlingType);
            }
        }
    }

    const CastlingData CastlingData::castlingData[NUM_CASTLE_TYPES]{
            { // white King side
                    maskOf(SQ_F1) | maskOf(SQ_G1),
                    maskOf(SQ_E1) | maskOf(SQ_F1) | maskOf(SQ_G1),
                    maskOf(SQ_E1) | maskOf(SQ_H1),
                    SQ_E1,
                    SQ_G1,
                    SQ_H1,
                    SQ_F1,
                    CASTLE_WHITE_KING_SIDE
            },

            { //white Queen side
                    maskOf(SQ_B1) | maskOf(SQ_C1) | maskOf(SQ_D1),
                    maskOf(SQ_D1) | maskOf(SQ_C1) | maskOf(SQ_E1),
                    maskOf(SQ_A1) | maskOf(SQ_E1),
                    SQ_E1,
                    SQ_C1,
                    SQ_A1,
                    SQ_D1,
                    CASTLE_WHITE_QUEEN_SIDE
            },

            { // black King Side
                    maskOf(SQ_F8) | maskOf(SQ_G8),
                    maskOf(SQ_E8) | maskOf(SQ_F8) | maskOf(SQ_G8),
                    maskOf(SQ_E8) | maskOf(SQ_H8),
                    SQ_E8,
                    SQ_G8,
                    SQ_H8,
                    SQ_F8,
                    CASTLE_BLACK_KING_SIDE

            },

            { // black Queen Side
                    maskOf(SQ_B8) | maskOf(SQ_C8) | maskOf(SQ_D8),
                    maskOf(SQ_D8) | maskOf(SQ_C8) | maskOf(SQ_E8),
                    maskOf(SQ_A8) | maskOf(SQ_E8),
                    SQ_E8,
                    SQ_C8,
                    SQ_A8,
                    SQ_D8,
                    CASTLE_BLACK_QUEEN_SIDE
            }
    };


    template<PieceType pieceType>
    void XrayData::setToSquare(Square square) {
        static_assert(pieceType == PIECE_TYPE_ROOK || pieceType == PIECE_TYPE_BISHOP, "Must be rook or bishop");
        SquareMask squareMask = maskOf(square);
        if constexpr (pieceType == PIECE_TYPE_ROOK) {
            direction1 = expandToEdgeInclusive<1, 0>(squareMask);
            direction2 = expandToEdgeInclusive<-1, 0>(squareMask);
            direction3 = expandToEdgeInclusive<0, 1>(squareMask);
            direction4 = expandToEdgeInclusive<0, -1>(squareMask);
        } else if constexpr (pieceType == PIECE_TYPE_BISHOP) {
            direction1 = expandToEdgeInclusive<1, 1>(squareMask);
            direction2 = expandToEdgeInclusive<-1, -1>(squareMask);
            direction3 = expandToEdgeInclusive<1, -1>(squareMask);
            direction4 = expandToEdgeInclusive<-1, 1>(squareMask);
        }
        allDirections = direction1 | direction2 |
                                 direction3 | direction4;
    }


    template<PieceType pieceType>
    inline Bitboard generateMoves_slow(Square square, Bitboard otherPieces);

    template<>
    inline Bitboard generateMoves_slow<PIECE_TYPE_BISHOP>(Square square, Bitboard otherPieces) {
        SquareMask squareMask = maskOf(square);
        return expandUntilCollision<1, 1>(squareMask, otherPieces) |
               expandUntilCollision<-1, 1>(squareMask, otherPieces) |
               expandUntilCollision<1, -1>(squareMask, otherPieces) |
               expandUntilCollision<-1, -1>(squareMask, otherPieces);
    }

    template<>
    inline Bitboard generateMoves_slow<PIECE_TYPE_ROOK>(Square square, Bitboard otherPieces) {
        SquareMask squareMask = maskOf(square);
        return expandUntilCollision<1, 0>(squareMask, otherPieces) |
               expandUntilCollision<-1, 0>(squareMask, otherPieces) |
               expandUntilCollision<0, 1>(squareMask, otherPieces) |
               expandUntilCollision<0, -1>(squareMask, otherPieces);
    }


    template<PieceType pieceType>
    inline Bitboard getBlockersMask(Square square);

    template<>
    inline Bitboard getBlockersMask<PIECE_TYPE_BISHOP>(Square square) {
        SquareMask squareMask = maskOf(square);
        return expandToEdgeExclusive<1, 1>(squareMask) |
               expandToEdgeExclusive<1, -1>(squareMask) |
               expandToEdgeExclusive<-1, 1>(squareMask) |
               expandToEdgeExclusive<-1, -1>(squareMask);
    }

    template<>
    inline Bitboard getBlockersMask<PIECE_TYPE_ROOK>(Square square) {
        SquareMask squareMask = maskOf(square);
        return expandToEdgeExclusive<1, 0>(squareMask) |
               expandToEdgeExclusive<-1, 0>(squareMask) |
               expandToEdgeExclusive<0, 1>(squareMask) |
               expandToEdgeExclusive<0, -1>(squareMask);
    }


    template<PieceType pieceType>
    inline Bitboard &seedOf(Square square);

    template<>
    inline Bitboard &seedOf<PIECE_TYPE_BISHOP>(Square square) {
        return bishopSeeds[square];
    }

    template<>
    inline Bitboard &seedOf<PIECE_TYPE_ROOK>(Square square) {
        return rookSeeds[square];
    }


    std::random_device bitboard_rd;
    std::default_random_engine bitboard_generator(bitboard_rd());
    std::uniform_int_distribution<long long unsigned> bitboard_distribution(BITBOARD_EMPTY, BITBOARD_FULL);

    Bitboard randomBitboard() {
        return bitboard_distribution(bitboard_generator);
    }

    std::random_device key_rd;
    std::default_random_engine key_generator(bitboard_rd());
    std::uniform_int_distribution<long long unsigned> key_distribution(KEY_ZERO, KEY_FULL);

    Key randomKey() {
        return static_cast<Key>(key_distribution(key_generator));
    }


    Bitboard randomBitboard_fewBits() {
        return randomBitboard() & randomBitboard();
    }

    const Bitboard rankShiftMasks[5] = {~maskOf(RANK_1) & ~maskOf(RANK_2),
                                        ~maskOf(RANK_1),
                                        BITBOARD_FULL,
                                        ~maskOf(RANK_8),
                                        ~maskOf(RANK_8) & ~maskOf(RANK_7)};


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

    template<int rankShift, int fileShift, bool includeEdge>
    Bitboard expandToEdge(Bitboard squareMask) {
        assert(squareMask_ok(static_cast<SquareMask>(squareMask)));
        constexpr int totalShift = fileShift + rankShift * NUM_FILES;
        Bitboard shiftMask = rankShiftMask<rankShift>() & fileShiftMask<fileShift>();
        Bitboard output = BITBOARD_EMPTY;
        squareMask &= shiftMask;
        while (squareMask) {
            squareMask = signedShift<totalShift>(squareMask);
            if constexpr (includeEdge) {
                output |= squareMask;
                squareMask &= shiftMask;
            } else {
                squareMask &= shiftMask;
                output |= squareMask;
            }
        }
        return output;
    }


    ///for every 1 in the bitboard, output an array where that one turns into a 0 or a 1
    ///eg. 10010 turns into [00000,10000,00010,10010]
    ///the length of the output array must be equal to 2^(number of 1s in the Bitboard)
    void toggleBits(Bitboard bitboard, Bitboard *outputArray, Bitboard *buffer) {
        int population = populationCout(bitboard);
        unsigned int numCombinations = 1u << population;
        Bitboard *individualSquares = buffer;
        Bitboard *ptr = individualSquares;
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

    int
    maxPossibleShift(const Bitboard *movesArray, Bitboard *hashedArray, unsigned int arrayLength, int previousBest) {
        int currentMaxShift = 63;
        for (int i = 0; i < arrayLength; i++) {
            for (int j = 0; j < arrayLength; j++) {
                if (movesArray[i] != movesArray[j]) {
                    int maxShift = maxShiftThatKeepsDifference(hashedArray[i], hashedArray[j]);
                    assert((hashedArray[i] >> maxShift) != (hashedArray[j] >> maxShift) || (maxShift == 0));
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

    struct LookUpData {
        Bitboard blockersMask;
        unsigned int numPermutations;
        Bitboard *blockers;
        Bitboard *legalMoves;
        Bitboard *hashed;
    };

    template<PieceType pieceType>
    LookUpData slidingPieceLookupInitBuffer(Square square, Bitboard *buffer) {
        Bitboard blockersMask = getBlockersMask<pieceType>(square);
        int population = populationCout(blockersMask);
        unsigned int numPermutations = 1u << population;
        Bitboard *blockers = buffer;
        Bitboard *legalMoves = buffer + numPermutations;
        Bitboard *hashed = buffer + 2 * numPermutations;
        Bitboard *toggleBitsBuffer = buffer + 3 * numPermutations;
        toggleBits(blockersMask, blockers, toggleBitsBuffer);
        for (int i = 0; i < numPermutations; ++i) {
            legalMoves[i] = generateMoves_slow<pieceType>(square, blockers[i]);
        }
        return LookUpData{
                blockersMask,
                numPermutations,
                blockers,
                legalMoves,
                hashed
        };
    }

    template<PieceType pieceType>
    void generateSeed(Square square, Bitboard *buffer, int numAttempts) {
        LookUpData lookUpData = slidingPieceLookupInitBuffer<pieceType>(square, buffer);
        Bitboard magicHashFactor = bruteForceSearchMagicHashFactor(lookUpData.numPermutations,
                                                                   lookUpData.blockers,
                                                                   lookUpData.legalMoves,
                                                                   lookUpData.hashed,
                                                                   numAttempts);
        magicHashAll(magicHashFactor, lookUpData.blockers, lookUpData.hashed, lookUpData.numPermutations);
        int shift = maxPossibleShift(lookUpData.legalMoves, lookUpData.hashed, lookUpData.numPermutations, 0);
        int maxIndex = 0;
        for (int i = 0; i < lookUpData.numPermutations; ++i) {
            lookUpData.hashed[i] >>= shift;
            if (lookUpData.hashed[i] > maxIndex) {
                maxIndex = lookUpData.hashed[i];
            }
        }
        currentLookupTableLength += maxIndex;
        seedOf<pieceType>(square) = magicHashFactor;

    }

    template<PieceType pieceType>
    void initSlidingPieceLookup(Square square, Bitboard *buffer) {
        LookUpData lookUpData = slidingPieceLookupInitBuffer<pieceType>(square, buffer);

        Bitboard magicHashFactor = seedOf<pieceType>(square);

        magicHashAll(magicHashFactor, lookUpData.blockers, lookUpData.hashed, lookUpData.numPermutations);
        int shift = maxPossibleShift(lookUpData.legalMoves, lookUpData.hashed, lookUpData.numPermutations, 0);

        int maxIndex = 0;
        for (int i = 0; i < lookUpData.numPermutations; ++i) {
            lookUpData.hashed[i] >>= shift;
            assert(currentBishopRookLookupPointer[lookUpData.hashed[i]] == BITBOARD_EMPTY);
            currentBishopRookLookupPointer[lookUpData.hashed[i]] = lookUpData.legalMoves[i];
            if (lookUpData.hashed[i] > maxIndex) {
                maxIndex = lookUpData.hashed[i];
            }
        }

        MagicHasData &magicHashData = slidingPieceDataOf<pieceType>(square).magicHashData;

        magicHashData.shift = shift;
        magicHashData.mask = lookUpData.blockersMask;
        magicHashData.magicHashFactor = magicHashFactor;
        magicHashData.moveLookup = currentBishopRookLookupPointer;


        SlidingPieceData& slidingPieceData = slidingPieceDataOf<pieceType>(square);
        slidingPieceData.xrayData.setToSquare<pieceType>(square);

        currentBishopRookLookupPointer += maxIndex + 1;
    }

    constexpr int maxLookupTablePopulation = 12;
    constexpr int maxLookupCombinations = 1u << maxLookupTablePopulation;
    constexpr int lookupTableBufferSize = maxLookupCombinations * 3 + maxLookupTablePopulation;

    void generateSeeds(int numAttempts) {
        Bitboard buffer[lookupTableBufferSize];
        SquareMask squareMask = SQUARE_MASK_FIRST;
        for (Square square = SQ_FIRST; square <= SQ_LAST; ++square, squareMask <<= 1) {
            generateSeed<PIECE_TYPE_BISHOP>(square, buffer, numAttempts);
            generateSeed<PIECE_TYPE_ROOK>(square, buffer, numAttempts);
        }

        cout << "Bishop Seeds: \n";
        cout << std::hex;
        printArray(bishopSeeds, NUM_SQUARES, "\n0x") << "\n\n";
        cout << "Rook Seeds: \n";
        printArray(rookSeeds, NUM_SQUARES, "\n0x") << "\n\n";

        cout << "Lookup table Length: " << currentLookupTableLength << "\n";
    }

    bool lookUpTablesReady = false;

    void initPieceMoveLookup() {
        if (!lookUpTablesReady) {
            Bitboard buffer[lookupTableBufferSize];
            currentBishopRookLookupPointer = rookBishopMoveTable;
            SquareMask squareMask = SQUARE_MASK_FIRST;
            for (Square square = SQ_FIRST; square <= SQ_LAST; ++square, squareMask <<= 1) {
                knightMovesLookup[square] = knightMovesFrom_slow(squareMask);
                kingMovesLookup[square] = kingMovesFrom_slow(squareMask);
                initSlidingPieceLookup<PIECE_TYPE_BISHOP>(square, buffer);
                initSlidingPieceLookup<PIECE_TYPE_ROOK>(square, buffer);
            }
            lookUpTablesReady = true;
        }
    }
}
