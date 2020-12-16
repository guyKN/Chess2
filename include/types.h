//
// Created by guykn on 12/7/2020.
//

#ifndef CHESS_TYPES_H
#define CHESS_TYPES_H

#include <cstdint>
#include <iosfwd>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <vector>

using std::vector;
using std::ostream;

#define ENABLE_INCR_OPERATORS_ON(T)                                \
inline T& operator++(T& d) { return d = T(int(d) + 1); }           \
inline T& operator--(T& d) { return d = T(int(d) - 1); }
namespace Chess {
    std::string removeSpaces(std::string &input);

    typedef uint64_t Bitboard;

    constexpr Bitboard BITBOARD_EMPTY = 0;
    constexpr Bitboard BITBOARD_FULL = ~BITBOARD_EMPTY;

//todo: implement
    std::ostream &printBitboard(Bitboard bitboard, std::ostream &os = std::cout);

    enum Square : int {
        SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
        SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
        SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
        SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
        SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
        SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
        SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
        SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
        SQ_FIRST = SQ_A1,
        SQ_LAST = SQ_H8,
        SQ_INVALID = -1
    };

    constexpr int NUM_SQUARES = 64;

    std::string toString(Square square);

    Square parseSquare(std::string &str);

    constexpr inline Square operator+(Square square1, Square square2) {
        return static_cast<Square>(static_cast<int>(square1) + static_cast<int>(square2));
    }

    constexpr bool square_ok(Square square) {
        return square >= SQ_FIRST && square <= SQ_LAST;
    }

    ENABLE_INCR_OPERATORS_ON(Square)

    enum Direction : int {
        NORTH = 8,
        SOUTH = -NORTH,
        EAST = 1,
        WEST = -EAST,

        NORTH_EAST = NORTH + EAST,
        NORTH_WEST = NORTH + WEST,
        SOUTH_EAST = SOUTH + EAST,
        SOUTH_WEST = SOUTH + WEST,

        NORTH_2 = 2 * NORTH,
        SOUTH_2 = 2 * SOUTH,
        EAST_2 = 2 * EAST,
        WEST_2 = 2 * WEST,

        KNIGHT_MOVE_1 = NORTH_2 + EAST,
        KNIGHT_MOVE_2 = NORTH_2 + WEST,
        KNIGHT_MOVE_3 = SOUTH_2 + EAST,
        KNIGHT_MOVE_4 = SOUTH_2 + WEST,
        KNIGHT_MOVE_5 = NORTH + EAST_2,
        KNIGHT_MOVE_6 = NORTH + WEST_2,
        KNIGHT_MOVE_7 = SOUTH + EAST_2,
        KNIGHT_MOVE_8 = SOUTH + WEST_2
    };

    constexpr inline Square operator+(Square square, Direction direction) {
        return static_cast<Square>(static_cast<int>(square) + static_cast<int>(direction));
    }

    constexpr inline Square operator-(Square square, Direction direction) {
        return static_cast<Square>(static_cast<int>(square) - static_cast<int>(direction));
    }

    constexpr inline Direction operator*(Direction direction, int scalar) {
        return static_cast<Direction>(static_cast<int>(direction) * scalar);
    }


    enum SquareMask : Bitboard {
        // Represents a Bitboard that contains all 0 except for exactly one 1 representing a square
        SQUARE_MASK_NONE = 0u,
        SQUARE_MASK_FIRST = 1u,
        SQUARE_MASK_LAST = SQUARE_MASK_FIRST >> SQ_LAST
    };

    inline constexpr SquareMask operator>>(SquareMask squareMask, int shift) {
        return static_cast<SquareMask>(static_cast<Bitboard>(squareMask) >> shift);
    }

    inline constexpr SquareMask operator<<(SquareMask squareMask, int shift) {
        return static_cast<SquareMask>(static_cast<Bitboard>(squareMask) << shift);
    }

    inline constexpr SquareMask operator<<=(SquareMask &squareMask, int shift) {
        squareMask = squareMask << shift;
        return squareMask;
    }

    inline constexpr SquareMask operator>>=(SquareMask &squareMask, int shift) {
        squareMask = squareMask >> shift;
        return squareMask;
    }

    constexpr inline SquareMask maskOf(Square square) {
        return static_cast<SquareMask>(SQUARE_MASK_FIRST << square);
    }

    bool squareMask_ok(SquareMask squareMask);

    enum NotSquareMask : Bitboard {
        NOT_SQUARE_MASK_NONE = ~SQUARE_MASK_NONE,
        NOT_SQUARE_MASK_FIRST = ~SQUARE_MASK_FIRST,
        NOT_SQUARE_MASK_LAST = ~SQUARE_MASK_LAST
    };

    inline constexpr NotSquareMask operator~(SquareMask squareMask) {
        return static_cast<NotSquareMask>(~static_cast<Bitboard>(squareMask));
    }

    inline constexpr SquareMask operator~(NotSquareMask notSquareMask) {
        return static_cast<SquareMask>(~static_cast<Bitboard>(notSquareMask));
    }


    constexpr inline NotSquareMask notSquareMask(Square square) {
        return ~maskOf(square);
    }

    bool notSquareMask_ok(NotSquareMask notSquareMask);


    enum Rank : int {
        RANK_1,
        RANK_2,
        RANK_3,
        RANK_4,
        RANK_5,
        RANK_6,
        RANK_7,
        RANK_8,
        RANK_FIRST = RANK_1,
        RANK_LAST = RANK_8,
        RANK_INVALID = -1
    };

    constexpr char RANK_NAMES[] = "12345678";

    char toChar(Rank rank);

    Rank parseRank(char rankChar);

    ENABLE_INCR_OPERATORS_ON(Rank)


    constexpr int NUM_RANKS = 8;

    constexpr bool rank_ok(Rank rank) {
        return rank >= RANK_FIRST && rank <= RANK_LAST;
    }

    inline Rank rankOf(Square square);

    enum File : int {
        FILE_A,
        FILE_B,
        FILE_C,
        FILE_D,
        FILE_E,
        FILE_F,
        FILE_G,
        FILE_H,
        FILE_FIRST = FILE_A,
        FILE_LAST = FILE_H,
        FILE_INVALID = -1
    };

    constexpr char FILE_NAMES[] = "ABCDEFGH";

    char toChar(File file);

    File parseFile(char file);

    ENABLE_INCR_OPERATORS_ON(File)

    constexpr int NUM_FILES = 8;

    constexpr inline bool file_ok(File file) {
        return file >= FILE_FIRST && file <= FILE_LAST;
    }

    inline constexpr File fileOf(Square square);

    inline constexpr Square makeSquare(Rank rank, File file) {
        return static_cast<Square>(rank * 8 + file);
    }

    enum Player : bool {
        WHITE = true,
        BLACK = false
    };

    constexpr int NUM_PLAYERS = 2;

    constexpr inline Player operator~(Player color) {
        return static_cast<Player>(!color);
    }

    constexpr inline int directionOf(Player player) {
        return player == WHITE ? 1 : -1;
    }

    constexpr inline Rank flipIfBlack(Player player, Rank rank) {
        return player == WHITE ? rank : static_cast<Rank>((RANK_LAST - rank));
    }



    constexpr Player STARTING_PLAYER = WHITE;

    enum PieceType : int {
        PIECE_TYPE_PAWN,
        PIECE_TYPE_KNIGHT,
        PIECE_TYPE_KING,
        PIECE_TYPE_NONE
    };

    enum Piece : int {
        PIECE_WHITE_PAWN,
        PIECE_WHITE_KNIGHT,
        PIECE_WHITE_KING,
        PIECE_BLACK_PAWN,
        PIECE_BLACK_KNIGHT,
        PIECE_BLACK_KING,
        PIECE_NONE,

        PIECE_FIRST = PIECE_WHITE_PAWN,
        PIECE_LAST = PIECE_NONE,
        PIECE_FIRST_WHITE = PIECE_WHITE_PAWN,
        PIECE_LAST_WHITE = PIECE_WHITE_KING,
        PIECE_FIRST_BLACK = PIECE_BLACK_PAWN,
        PIECE_LAST_BLACK = PIECE_BLACK_KING,
        PIECE_INVALID = -1
    };


    ENABLE_INCR_OPERATORS_ON(Piece)


    constexpr int NUM_PIECES = PIECE_LAST-PIECE_FIRST+1;

    constexpr inline Player playerOf(Piece piece) {
        if (piece >= PIECE_FIRST_WHITE && piece <= PIECE_LAST_WHITE) {
            return WHITE;
        } else if (piece >= PIECE_FIRST_BLACK && piece <= PIECE_LAST_BLACK) {
            return BLACK;
        }
        assert(false);
        return WHITE;
    }

    constexpr inline bool piece_ok(Piece piece) {
        return piece >= PIECE_FIRST && piece <= PIECE_LAST;
    }

    constexpr inline Piece firstPieceOf(Player player) {
        return player == WHITE ? PIECE_FIRST_WHITE : PIECE_FIRST_BLACK;
    }

    constexpr inline Piece lastPieceOf(Player player) {
        return player == WHITE ? PIECE_LAST_WHITE : PIECE_LAST_BLACK;
    }



    char toChar(Piece piece);

    Piece parsePiece(char pieceChar);

    constexpr inline Piece makePiece(PieceType pieceType, Player player) {
        assert(pieceType != PIECE_TYPE_NONE);
        return static_cast<Piece>(firstPieceOf(player) + pieceType);
    }

}
#endif //CHESS_TYPES_H
