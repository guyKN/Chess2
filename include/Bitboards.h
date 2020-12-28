//
// Created by guykn on 12/11/2020.
//

#ifndef CHESS_BITBOARD_H
#define CHESS_BITBOARD_H

#include <random>
#include <types.h>

namespace Chess {

    void initBitboards();


    constexpr Bitboard MASK_FILE_A = 0x0101010101010101;
    constexpr Bitboard MASK_RANK_1 = 0x00000000000000FF;

    enum CastlingType {
        CASTLE_WHITE_KING_SIDE,
        CASTLE_WHITE_QUEEN_SIDE,
        CASTLE_BLACK_KING_SIDE,
        CASTLE_BLACK_QUEEN_SIDE,
        CASTLE_NONE
    };

    inline Player playerOf(CastlingType castlingType) {
        assert(castlingType != CASTLE_NONE);
        return ((castlingType == CASTLE_WHITE_KING_SIDE) || (castlingType == CASTLE_WHITE_QUEEN_SIDE)) ? WHITE : BLACK;
    }

    inline bool isKingSide(CastlingType castlingType){
        assert(castlingType != CASTLE_NONE);
        return (castlingType == CASTLE_WHITE_KING_SIDE) || (castlingType == CASTLE_BLACK_KING_SIDE);
    }

    inline bool isQueenSde(CastlingType castlingType){
        return !isKingSide(castlingType);
    }

    template<Player player>
    inline CastlingType kingSideCastleOf();

    template<>
    inline CastlingType kingSideCastleOf<WHITE>(){
        return CASTLE_WHITE_KING_SIDE;
    }

    template<>
    inline CastlingType kingSideCastleOf<BLACK>(){
        return CASTLE_BLACK_KING_SIDE;
    }

    template<Player player>
    inline CastlingType queenSideCastleOf();

    template<>
    inline CastlingType queenSideCastleOf<WHITE>(){
        return CASTLE_WHITE_QUEEN_SIDE;
    }

    template<>
    inline CastlingType queenSideCastleOf<BLACK>(){
        return CASTLE_BLACK_QUEEN_SIDE;
    }

    static constexpr int NUM_CASTLE_TYPES = 4;

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

        inline bool mayCastle(Bitboard pieces, Bitboard threats) const {
            return !(pieces & mustBeEmpty) && !(threats & mustNotBeInCheck);
        }

        static inline CastlingData fromCastlingType(CastlingType castlingType) {
            assert(castlingType >= 0 && castlingType < NUM_CASTLE_TYPES);
            return castlingData[castlingType];
        }

        template<Player player>
        static inline CastlingData kingSideCastleOf() {
            return fromCastlingType(Chess::kingSideCastleOf<player>());
        }

        template<Player player>
        static inline CastlingData queenSideCastleOf() {
            return fromCastlingType(Chess::queenSideCastleOf<player>());
        }

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
    };

    struct XrayData {
        Bitboard direction1;
        Bitboard direction2;
        Bitboard direction3;
        Bitboard direction4;
        Bitboard allDirections;

        XrayData() = default;

        template<PieceType pieceType>
        static XrayData fromSquare(Square square);

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
                assert(false);
                return BITBOARD_EMPTY;
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

    Bitboard randomBitboard();

    Bitboard randomBitboard_fewBits();


/// code below shamelessly stolen from stockfish
/// https://github.com/official-stockfish/Stockfish
/// thank you stockfish



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

#elif defined(_MSC_VER)  // MSVC

#ifdef _WIN64  // MSVC, WIN64

    inline Square lsb(Bitboard b) {
        assert(b);
        unsigned long idx;
        _BitScanForward64(&idx, b);
        return static_cast<Square>(idx);
    }

    inline Square msb(Bitboard b) {
        assert(b);
        unsigned long idx;
        _BitScanReverse64(&idx, b);
        return static_cast<Square>(idx);
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


/// frontmost_sq() returns the most advanced pawnForward2Square for the given color,
/// requires a non-zero bitboard.
    inline Square frontmost_sq(Player c, Bitboard b) {
        assert(b);
        return c == WHITE ? msb(b) : lsb(b);
    }


}
#endif //CHESS_BITBOARD_H

