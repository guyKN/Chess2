//
// Created by guykn on 12/11/2020.
//

#ifndef CHESS_BITBOARD_H
#define CHESS_BITBOARD_H

#include "ChessBoard.h"

namespace Chess {

    //todo:remove
    Bitboard rookRelevantSquaresMask(Square square);
    Bitboard bishopRelevantSquaresMask(Square square);
    Bitboard toggleBits(Bitboard bitboard, Bitboard* outputArray);
    Bitboard rookMovesFrom_slow(Square square, Bitboard otherPieces);
    Bitboard bishopMovesFrom_slow(Square square, Bitboard otherPieces);
    Bitboard queenMovesFrom_slow(Square square, Bitboard otherPieces);


    void initLookupTables();

    const extern Bitboard rankMasks[NUM_RANKS];

    const extern Bitboard fileMasks[NUM_FILES];

    const extern Bitboard fileShiftMasks[5];
    const extern Bitboard rankShiftMasks[5];

    extern Bitboard knightMovesLookup[NUM_SQUARES];
    extern Bitboard kingMovesLookup[NUM_SQUARES];

    inline Bitboard knightMovesFrom(Square square) {
        return knightMovesLookup[square];
    }


    inline Bitboard kingMovesFrom(Square square) {
        return kingMovesLookup[square];
    }

    inline Bitboard maskOfFile(File file) {
        assert(file_ok(file));
        return fileMasks[file];
    }

    inline Bitboard maskOfRank(Rank rank) {
        assert(rank_ok(rank));
        return rankMasks[rank];
    }

    template<int fileShift>
    inline Bitboard fileShiftMask() {
        assert(fileShift>=-2 && fileShift <=2);
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


/// code below shamelessly stolen from stockfish
/// https://github.com/official-stockfish/Stockfish
/// thank you stockfish



/// populationCout() counts the number of non-zero bits in a bitboard
//todo: use a faster popCount

    inline int populationCout(Bitboard b) {


#if defined(_MSC_VER) || defined(__INTEL_COMPILER)

        return (int)_mm_popcnt_u64(b);

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


/// frontmost_sq() returns the most advanced square for the given color,
/// requires a non-zero bitboard.
    inline Square frontmost_sq(Player c, Bitboard b) {
        assert(b);
        return c == WHITE ? msb(b) : lsb(b);
    }


}
#endif //CHESS_BITBOARD_H

