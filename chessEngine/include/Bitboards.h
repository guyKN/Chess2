//
// Created by guykn on 12/11/2020.
//

#ifndef CHESS_BITBOARD_H
#define CHESS_BITBOARD_H

#include <random>
#include <types.h>
#include <stdexcept>

#if defined(_MSC_VER) || defined(__INTEL_COMPILER)

#include <nmmintrin.h>
#include <intrin.h>

#endif


namespace Chess {
    void initPieceMoveLookup();

    void generateSeeds(int numAttempts);


    constexpr Bitboard MASK_FILE_A = 0x0101010101010101;
    constexpr Bitboard MASK_RANK_1 = 0x00000000000000FF;

    enum CastlingType {
        CASTLE_WHITE_KING_SIDE,
        CASTLE_WHITE_QUEEN_SIDE,
        CASTLE_BLACK_KING_SIDE,
        CASTLE_BLACK_QUEEN_SIDE,
        CASTLE_NONE,
        CASTLE_FIRST = CASTLE_WHITE_KING_SIDE,
        CASTLE_LAST = CASTLE_BLACK_QUEEN_SIDE
    };

    constexpr int NUM_CASTLE_TYPES = 4;

    constexpr CastlingType operator++(CastlingType &castlingType) {
        return castlingType = static_cast<CastlingType>(castlingType + 1);
    }

    inline Player playerOf(CastlingType castlingType) {
        assert(castlingType != CASTLE_NONE);
        return ((castlingType == CASTLE_WHITE_KING_SIDE) || (castlingType == CASTLE_WHITE_QUEEN_SIDE)) ? WHITE : BLACK;
    }

    inline bool isKingSide(CastlingType castlingType) {
        assert(castlingType != CASTLE_NONE);
        return (castlingType == CASTLE_WHITE_KING_SIDE) || (castlingType == CASTLE_BLACK_KING_SIDE);
    }

    inline bool isQueenSde(CastlingType castlingType) {
        return !isKingSide(castlingType);
    }

    template<Player player>
    inline CastlingType kingSideCastleOf();

    template<>
    inline CastlingType kingSideCastleOf<WHITE>() {
        return CASTLE_WHITE_KING_SIDE;
    }

    template<>
    inline CastlingType kingSideCastleOf<BLACK>() {
        return CASTLE_BLACK_KING_SIDE;
    }

    template<Player player>
    inline CastlingType queenSideCastleOf();

    template<>
    inline CastlingType queenSideCastleOf<WHITE>() {
        return CASTLE_WHITE_QUEEN_SIDE;
    }

    template<>
    inline CastlingType queenSideCastleOf<BLACK>() {
        return CASTLE_BLACK_QUEEN_SIDE;
    }

    enum CastlingRights : unsigned int {
        CASTLE_RIGHTS_NONE = 0,
        CASTLE_RIGHTS_WHITE_KING_SIDE = 1u << CASTLE_WHITE_KING_SIDE,
        CASTLE_RIGHTS_WHITE_QUEEN_SIDE = 1u << CASTLE_WHITE_QUEEN_SIDE,
        CASTLE_RIGHTS_BLACK_KING_SIDE = 1u << CASTLE_BLACK_KING_SIDE,
        CASTLE_RIGHTS_BLACK_QUEEN_SIDE = 1u << CASTLE_BLACK_QUEEN_SIDE,
        CASTLE_RIGHTS_ALL = CASTLE_RIGHTS_WHITE_KING_SIDE | CASTLE_RIGHTS_WHITE_QUEEN_SIDE |
                            CASTLE_RIGHTS_BLACK_KING_SIDE | CASTLE_RIGHTS_BLACK_QUEEN_SIDE
    };

    constexpr unsigned int NUM_CASTLING_RIGHTS = 16;

    inline constexpr CastlingRights operator&(CastlingRights castlingRights1, CastlingRights castlingRights2) {
        return static_cast<CastlingRights>(static_cast<unsigned int>(castlingRights1) &
                                           static_cast<unsigned int>(castlingRights2));
    }

    inline constexpr CastlingRights &operator&=(CastlingRights &castlingRights1, CastlingRights castlingRights2) {
        return castlingRights1 = castlingRights1 & castlingRights2;
    }

    inline constexpr CastlingRights operator|(CastlingRights castlingRights1, CastlingRights castlingRights2) {
        return static_cast<CastlingRights>(static_cast<unsigned int>(castlingRights1) |
                                           static_cast<unsigned int>(castlingRights2));
    }

    inline constexpr CastlingRights &operator|=(CastlingRights &castlingRights1, CastlingRights castlingRights2) {
        return castlingRights1 = castlingRights1 | castlingRights2;
    }


    inline constexpr CastlingRights operator~(CastlingRights castlingRights) {
        return static_cast<CastlingRights>(~static_cast<unsigned int>(castlingRights));
    }


    inline constexpr CastlingRights castlingRightsOf(CastlingType castlingType) {
        return static_cast<CastlingRights>(1u << castlingType);
    }


    struct CastlingData {
        const Bitboard mustBeEmpty;
        const Bitboard mustNotBeInCheck;
        const Bitboard disableCastlingMask;
        const Square kingSrc;
        const Square kingDst;
        const Square rookSrc;
        const Square rookDst;
        const CastlingType castlingType;

        inline bool moveDisablesCastling(Bitboard moveSquares) const {
            return moveSquares & disableCastlingMask;
        }

        CastlingData(const CastlingData &) = delete;

        CastlingData(const CastlingData &&) = delete;

        CastlingData &operator=(const CastlingData &) = delete;

        CastlingData &operator=(const CastlingData &&) = delete;

        inline bool mayCastle(Bitboard pieces, Bitboard threats) const {
            return !(pieces & mustBeEmpty) && !(threats & mustNotBeInCheck);
        }

        static inline const CastlingData &fromCastlingType(CastlingType castlingType) {
            assert(castlingType >= 0 && castlingType < NUM_CASTLE_TYPES);
            return castlingData[castlingType];
        }

        template<Player player>
        static inline const CastlingData &kingSideCastleOf() {
            return fromCastlingType(Chess::kingSideCastleOf<player>());
        }

        template<Player player>
        static inline const CastlingData &queenSideCastleOf() {
            return fromCastlingType(Chess::queenSideCastleOf<player>());
        }

        static void updateCastlingRights(Bitboard moveSquares, CastlingRights &castlingRightsMask);

    private:
        static const CastlingData castlingData[NUM_CASTLE_TYPES];
    };

    struct MagicHasData {
        int shift;
        Bitboard mask;
        Bitboard magicHashFactor;
        Bitboard *moveLookup;

        //todo: should this be inline?
        inline Bitboard calculateSlidingMoves(Bitboard otherPieces) {
            otherPieces &= mask;
            otherPieces *= magicHashFactor;
            otherPieces >>= shift;
            return moveLookup[otherPieces];
        }

        MagicHasData() = default;

        MagicHasData(const MagicHasData &) = delete;

        MagicHasData(const MagicHasData &&) = delete;

        MagicHasData &operator=(const MagicHasData &) = delete;

        MagicHasData &operator=(const MagicHasData &&) = delete;

    };

    struct XrayData {
        Bitboard direction1;
        Bitboard direction2;
        Bitboard direction3;
        Bitboard direction4;
        Bitboard allDirections;

        XrayData() = default;

        XrayData(const XrayData &) = delete;

        XrayData(const XrayData &&) = delete;

        XrayData &operator=(const XrayData &) = delete;

        XrayData &operator=(const XrayData &&) = delete;


        template<PieceType pieceType>
        void setToSquare(Square square);

        inline Bitboard directionTo(Bitboard squareMask) {
            if (squareMask & direction1) {
                return direction1;
            } else if (squareMask & direction2) {
                return direction2;
            } else if (squareMask & direction3) {
                return direction3;
            } else if (squareMask & direction4) {
                //todo: assume it can be direction four without checking, for faster performance
                return direction4;
            } else {
                // assert(false); // todo: bring back AFTER debugging
                return BITBOARD_FULL;
            }
        }

        inline Bitboard rankFileDiagonal1() {
            return direction1 | direction2;
        }

        inline Bitboard rankFileDiagonal2() {
            return direction3 | direction4;
        }
    };

    struct SlidingPieceData {
        MagicHasData magicHashData;
        XrayData xrayData;

        SlidingPieceData() = default;

        SlidingPieceData(const SlidingPieceData &) = delete;

        SlidingPieceData(const SlidingPieceData &&) = delete;

        SlidingPieceData &operator=(const SlidingPieceData &) = delete;

        SlidingPieceData &operator=(const SlidingPieceData &&) = delete;

    };


    extern Bitboard knightMovesLookup[NUM_SQUARES];
    extern Bitboard kingMovesLookup[NUM_SQUARES];

    extern SlidingPieceData bishopData[NUM_SQUARES];
    extern SlidingPieceData rookData[NUM_SQUARES];

    extern Bitboard rookBishopMoveTable[];

    const extern Bitboard fileShiftMasks[5];

    const extern Bitboard rankShiftMasks[5];

    inline Bitboard knightMovesFrom(Square square) {
        return knightMovesLookup[square];
    }


    inline Bitboard kingMovesFrom(Square square) {
        return kingMovesLookup[square];
    }

    constexpr inline Bitboard maskOf(File file) {
        assert(file_ok(file));
        return MASK_FILE_A << file;
    }

    constexpr inline Bitboard maskOf(Rank rank) {
        assert(rank_ok(rank));
        return MASK_RANK_1 << (rank * 8);
    }

    template<PieceType pieceType>
    inline SlidingPieceData &slidingPieceDataOf(Square square);

    template<>
    inline SlidingPieceData &slidingPieceDataOf<PIECE_TYPE_BISHOP>(Square square) {
        return bishopData[square];
    }

    template<>
    inline SlidingPieceData &slidingPieceDataOf<PIECE_TYPE_ROOK>(Square square) {
        return rookData[square];
    }

    template<PieceType pieceType>
    inline Bitboard slidingMovesFrom(Square square, Bitboard otherPieces) {
        return slidingPieceDataOf<pieceType>(square).magicHashData.calculateSlidingMoves(otherPieces);
    }

    template<>
    inline Bitboard slidingMovesFrom<PIECE_TYPE_QUEEN>(Square square, Bitboard otherPieces) {
        return slidingMovesFrom<PIECE_TYPE_ROOK>(square, otherPieces) |
               slidingMovesFrom<PIECE_TYPE_BISHOP>(square, otherPieces);
    }

    template<int fileShift>
    inline Bitboard fileShiftMask() {
        assert(fileShift >= -2 && fileShift <= 2);
        return fileShiftMasks[fileShift + 2];
    }

    template<int rankShift>
    inline Bitboard rankShiftMask() {
        static_assert(rankShift <= 2 && rankShift >= -2);
        constexpr int index = rankShift + 2;
        return rankShiftMasks[index];
    }

    template<int shift>
    constexpr Bitboard signedShift(Bitboard bitboard) {
        if constexpr (shift > 0) {
            return bitboard << shift;
        } else if constexpr (shift < 0) {
            return bitboard >> (-shift);
        } else {
            return bitboard;
        }
    }

    template<int rankShift, int fileShift>
    inline Bitboard shiftWithMask(Bitboard bitboard) {
        static_assert(rankShift > -NUM_RANKS &&
                      rankShift < NUM_RANKS &&
                      fileShift > -NUM_FILES &&
                      fileShift < NUM_FILES,
                      "file and rank shiftWithMask must be between -7 and 7");
        constexpr int totalShift = fileShift + rankShift * NUM_FILES;
        Bitboard mask = fileShiftMask<fileShift>();
        return signedShift<totalShift>(bitboard & mask);
    }

    inline Key firstBits(int numBits) {
        return static_cast<Key>((1u << numBits) - 1);
    }

    Bitboard randomBitboard();

    Bitboard randomBitboard_fewBits();

    Key randomKey();

/// code below shamelessly stolen from stockfish
/// https://github.com/official-stockfish/Stockfish
/// thank you stockfish


/// xorshift64star Pseudo-Random Number Generator
/// This class is based on original code written and dedicated
/// to the public domain by Sebastiano Vigna (2014).
/// It has the following characteristics:
///
///  -  Outputs 64-bit numbers
///  -  Passes Dieharder and SmallCrush test batteries
///  -  Does not require warm-up, no zeroland to escape
///  -  Internal state is a single 64-bit integer
///  -  Period is 2^64 - 1
///  -  Speed: 1.60 ns/call (Core i7 @3.40GHz)
///
/// For further analysis see
///   <http://vigna.di.unimi.it/ftp/papers/xorshift.pdf>

    class PRNG {

        uint64_t s;

        uint64_t rand64() {

            s ^= s >> 12, s ^= s << 25, s ^= s >> 27;
            return s * 2685821657736338717LL;
        }

    public:
        PRNG(uint64_t seed) : s(seed) { assert(seed); }

        template<typename T>
        T rand() { return T(rand64()); }

        /// Special generator used to fast init magic numbers.
        /// Output values only have 1/8th of their bits set on average.
        template<typename T>
        T sparse_rand() { return T(rand64() & rand64() & rand64()); }
    };



/// populationCout() counts the number of non-zero bits in a bitboard
//todo: use a faster popCount

    inline int populationCout(Bitboard b) {


#if defined(_MSC_VER) || defined(__INTEL_COMPILER)

        return (int) _mm_popcnt_u64(b);

#else // Assumed gcc or compatible compiler

        return __builtin_popcountll(b);

#endif
    }


/// lsb() and msb() return the least/most significant bit in a non-zero bitboard

#if defined(__GNUC__)  // GCC, Clang, ICC

    inline Square lsb(Bitboard b) {
      assert(b);
      return Square(__builtin_ctzll(b));
    }

    inline Square msb(Bitboard b) {
      assert(b);
      return Square(63 ^ __builtin_clzll(b));
    }

    inline Piece lsb(ThreatMap threatMap) {
        assert(threatMap);
        unsigned long index;
        return static_cast<Piece>(__builtin_ctzll(threatMap));
    }


#elif defined(_MSC_VER)  // MSVC

#ifdef _WIN64  // MSVC, WIN64

    inline Square lsb(Bitboard b) {
        assert(b);
        unsigned long index;
        _BitScanForward64(&index, b);
        return static_cast<Square>(index);
    }

    inline Square msb(Bitboard b) {
        assert(b);
        unsigned long idx;
        _BitScanReverse64(&idx, b);
        return static_cast<Square>(idx);
    }

    inline Piece lsb(ThreatMap threatMap) {
        assert(threatMap);
        unsigned long index;
        _BitScanForward(&index, threatMap);
        return static_cast<Piece>(index);
    }

#else  // MSVC, WIN32

    inline Square lsb(Bitboard b) {
        assert(b);
        unsigned long idx;

        if (b & 0xffffffff) {
            _BitScanForward(&idx, int32_t(b));
            return Square(idx);
        } else {
            _BitScanForward(&idx, int32_t(b >> 32));
            return Square(idx + 32);
        }
    }

    inline Square msb(Bitboard b) {
        assert(b);
        unsigned long idx;

        if (b >> 32) {
            _BitScanReverse(&idx, int32_t(b >> 32));
            return Square(idx + 32);
        } else {
            _BitScanReverse(&idx, int32_t(b));
            return Square(idx);
        }
    }

#endif

#else  // Compiler is neither GCC nor MSVC compatible

#error "Compiler not supported."

#endif

/// popLsb() finds and clears the least significant bit in a non-zero bitboard
//todo: check if passing by refrence mat be faster
    inline Square popLsb(Bitboard *b) {
        assert(*b);
        const Square s = lsb(*b);
        *b &= *b - 1;
        return s;
    }

    inline SquareMask popLsbMask(Bitboard *b) {
        //todo: is this fastest
        return maskOf(popLsb(b));
    }


/// frontmost_sq() returns the most advanced Square for the given color,
/// requires a non-zero bitboard.
    inline Square frontmost_sq(Player c, Bitboard b) {
        assert(b);
        return c == WHITE ? msb(b) : lsb(b);
    }

    inline Piece popLsb(ThreatMap &threatMap, Player player) {
        assert(threatMap);
        Piece piece = lsb(threatMap & threatMapOf(player));
        threatMap ^= threatMapOf(piece);
        return piece;
    }

}
#endif //CHESS_BITBOARD_H