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

    enum Key : uint64_t {
        KEY_ZERO = 0,
        KEY_FULL = ~KEY_ZERO
    };

    constexpr inline Key operator^(Key key1, Key key2) {
        return static_cast<Key>(static_cast<uint64_t>(key1) ^ static_cast<uint64_t>(key2));
    }

    constexpr inline Key &operator^=(Key &key1, Key key2) {
        return key1 = key1 ^ key2;
    }

    constexpr inline Key operator&(Key key1, Key key2) {
        return static_cast<Key>(static_cast<uint64_t>(key1) & static_cast<uint64_t>(key2));
    }

    constexpr inline Key &operator&=(Key &key1, Key key2) {
        return key1 = key1 & key2;
    }


    constexpr Bitboard BITBOARD_EMPTY = 0;
    constexpr Bitboard BITBOARD_FULL = ~BITBOARD_EMPTY;

    std::ostream &printBitboard(Bitboard bitboard, std::ostream &os = std::cout);

    template<typename T>
    std::ostream &printArray(T *array, int length, const std::string &prefix = "", ostream &outputStream = std::cout) {
        outputStream << '{';
        for (int i = 0; i < length; i++) {
            outputStream << prefix << array[i] << ", ";
        }
        outputStream << '}' << std::dec;
        return outputStream;
    }

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
        SQ_INVALID = SQ_LAST + 1
    };

    constexpr int NUM_SQUARES = 64;

    std::string toString(Square square);

    Square parseSquare(std::string &str);

    constexpr inline Square operator+(Square square1, Square square2) {
        return static_cast<Square>(static_cast<int>(square1) + static_cast<int>(square2));
    }

    constexpr inline Square operator+(Square square1, int direction) {
        return static_cast<Square>(static_cast<int>(square1) + direction);
    }

    constexpr inline Square operator+(int direction, Square square) {
        return static_cast<Square>(static_cast<int>(square) + direction);
    }

    constexpr inline Square operator-(Square square1, int shift) {
        return static_cast<Square>(static_cast<int>(square1) - shift);
    }


    constexpr bool square_ok(Square square) {
        return square >= SQ_FIRST && square <= SQ_LAST;
    }

    ENABLE_INCR_OPERATORS_ON(Square)


    enum SquareMask : Bitboard {
        // Represents a Bitboard that contains all 0 except for exactly one 1 representing a pawnForward2Square
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
        return static_cast<SquareMask>(static_cast<Bitboard>(SQUARE_MASK_FIRST) << static_cast<unsigned int>(square));
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
        RANK_INVALID = -1,
    };

    constexpr char RANK_NAMES[] = "12345678";

    char toChar(Rank rank);

    inline std::ostream &operator<<(std::ostream &os, Rank rank) {
        return os << toChar(rank);
    }

    Rank parseRank(char rankChar);

    ENABLE_INCR_OPERATORS_ON(Rank)

    constexpr int NUM_RANKS = 8;

    constexpr bool rank_ok(Rank rank) {
        return rank >= RANK_FIRST && rank <= RANK_LAST;
    }

    constexpr inline Rank flip(Rank rank) {
        return static_cast<Rank>(RANK_LAST - rank);
    }

    constexpr inline Rank rankOf(Square square) {
        return static_cast<Rank>(square / 8);
    }

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
        FILE_AFTER_LAST = FILE_LAST + 1,
        FILE_INVALID = -1
    };

    constexpr char FILE_NAMES[] = "abcdefgh";

    char toChar(File file);

    inline std::ostream &operator<<(std::ostream &os, File file) {
        return os << toChar(file);
    }

    File parseFile(char file);

    ENABLE_INCR_OPERATORS_ON(File)

    constexpr int NUM_FILES = 8;

    constexpr inline bool file_ok(File file) {
        return file >= FILE_FIRST && file <= FILE_LAST;
    }

    inline constexpr File fileOf(Square square) {
        //todo: see if this optimizes
        return static_cast<File>(square % 8);
    }

    constexpr inline File flip(File file) {
        return static_cast<File>(FILE_LAST - file);
    }


    inline constexpr Square makeSquare(Rank rank, File file) {
        return static_cast<Square>(rank * 8 + file);
    }

    inline std::ostream &operator<<(std::ostream &os, Square square) {
        return os << fileOf(square) << rankOf(square);
    }


    enum Player : bool {
        WHITE = true,
        BLACK = false
    };

    inline constexpr int multiplierOf(Player player) {
        return player == WHITE ? 1 : -1;
    }

    std::ostream &operator<<(std::ostream &outputStream, Player player);

    constexpr int NUM_PLAYERS = 2;

    constexpr inline Player operator~(Player color) {
        return static_cast<Player>(!color);
    }

    constexpr inline int directionOf(Player player) {
        return player == WHITE ? 1 : -1;
    }

    constexpr inline Rank flipIfBlack(Player player, Rank rank) {
        return player == WHITE ? rank : flip(rank);
    }

    constexpr inline Square flip(Square square) {
        //note: not optimized. do not call except during setup
        return makeSquare(flip(rankOf(square)), fileOf(square));
    }


    constexpr Player STARTING_PLAYER = WHITE;

    enum PieceType : int {
        PIECE_TYPE_PAWN,
        PIECE_TYPE_KNIGHT,
        PIECE_TYPE_BISHOP,
        PIECE_TYPE_ROOK,
        PIECE_TYPE_QUEEN,
        PIECE_TYPE_KING,
        PIECE_TYPE_NONE,
        PIECE_TYPE_INVALID,
        PIECE_TYPE_FIRST = PIECE_TYPE_PAWN,
        PIECE_TYPE_LAST_NOT_NONE = PIECE_TYPE_KING,
        PIECE_TYPE_LAST = PIECE_TYPE_NONE
    };

    constexpr inline bool pieceTypeOk(PieceType pieceType) {
        return pieceType >= PIECE_TYPE_FIRST && pieceType <= PIECE_TYPE_LAST;
    }

    inline PieceType &operator++(PieceType &pieceType) {
        return pieceType = static_cast<PieceType>(pieceType + 1);
    }

    constexpr int NUM_PIECE_TYPE = 6;

    PieceType parsePieceType(char c);

    constexpr bool isValidPromotion(PieceType pieceType) {
        switch (pieceType) {
            case PIECE_TYPE_KNIGHT:
            case PIECE_TYPE_BISHOP:
            case PIECE_TYPE_ROOK:
            case PIECE_TYPE_QUEEN:
                return true;
            default:
                return false;
        }
    }

    enum Piece : int {
        PIECE_WHITE_PAWN,
        PIECE_WHITE_KNIGHT,
        PIECE_WHITE_BISHOP,
        PIECE_WHITE_ROOK,
        PIECE_WHITE_QUEEN,
        PIECE_WHITE_KING,

        PIECE_BLACK_PAWN,
        PIECE_BLACK_KNIGHT,
        PIECE_BLACK_BISHOP,
        PIECE_BLACK_ROOK,
        PIECE_BLACK_QUEEN,
        PIECE_BLACK_KING,

        PIECE_NONE,

        PIECE_FIRST = PIECE_WHITE_PAWN,
        PIECE_LAST = PIECE_NONE,
        PIECE_FIRST_WHITE = PIECE_WHITE_PAWN,
        PIECE_LAST_WHITE = PIECE_WHITE_KING,
        PIECE_FIRST_BLACK = PIECE_BLACK_PAWN,
        PIECE_LAST_BLACK = PIECE_BLACK_KING,
        PIECE_LAST_NOT_EMPTY = PIECE_LAST_BLACK,
        PIECE_INVALID = -1
    };

    ENABLE_INCR_OPERATORS_ON(Piece)

    char toChar(Piece piece);

    inline ostream &operator<<(ostream &os, Piece piece) {
        return os << toChar(piece);
    }

    char toChar(PieceType pieceType);

    Piece parsePiece(char pieceChar);

    constexpr int NUM_PIECES = PIECE_LAST - PIECE_FIRST + 1;

    constexpr int NUM_NON_EMPTY_PIECES = NUM_PIECES - 1;

    constexpr inline Player playerOf(Piece piece) {
        if (piece >= PIECE_FIRST_WHITE && piece <= PIECE_LAST_WHITE) {
            return WHITE;
        } else if (piece >= PIECE_FIRST_BLACK && piece <= PIECE_LAST_BLACK) {
            return BLACK;
        }
        assert(false);
        return WHITE;
    }

    constexpr inline bool pieceOk(Piece piece) {
        return piece >= PIECE_FIRST && piece <= PIECE_LAST;
    }

    constexpr inline Piece firstPieceOf(Player player) {
        return player == WHITE ? PIECE_FIRST_WHITE : PIECE_FIRST_BLACK;
    }

    constexpr inline Piece lastPieceOf(Player player) {
        return player == WHITE ? PIECE_LAST_WHITE : PIECE_LAST_BLACK;
    }

    constexpr inline Piece makePiece(PieceType pieceType, Player player) {
        assert(pieceType != PIECE_TYPE_NONE);
        return static_cast<Piece>(firstPieceOf(player) + pieceType);
    }


    constexpr inline PieceType pieceTypeOf(Piece piece) {
        if (piece == PIECE_NONE) {
            return PIECE_TYPE_NONE;
        } else if (playerOf(piece) == WHITE) {
            return static_cast<PieceType>(piece);
        } else {
            return static_cast<PieceType>(piece - PIECE_FIRST_BLACK);
        }
    }

    constexpr inline Piece flip(Piece piece) {
        Player player = playerOf(piece);
        return (player == WHITE) ?
               static_cast<Piece>(piece + NUM_PIECE_TYPE) :
               static_cast<Piece>(piece - NUM_PIECE_TYPE);
    }

    enum WinState : int {
        BLACK_WINS,
        WHITE_WINS,
        NO_WINNER,
        WIN_STATE_DRAW
    };

    inline WinState winStateFromPlayer(Player player) {
        return static_cast<WinState>(player);
    }

    constexpr int MAX_DEPTH = 512;

    enum Score : int {
        SCORE_DRAW = 0,
        SCORE_ZERO = 0,
        SCORE_MATE = 1'000'000,
        SCORE_KNOWN_WIN = SCORE_MATE - MAX_DEPTH,
        SCORE_MATED = -SCORE_MATE,
        SCORE_KNOWN_LOSS = -SCORE_KNOWN_WIN,
        SCORE_INFINITY = SCORE_MATE + 1,
    };

    enum GameEndState {
        NO_GAME_END,
        DRAW,
        MATED
    };

    inline constexpr WinState winStateOf(GameEndState gameEndState, Player currentPlayer) {
        switch (gameEndState) {
            case DRAW:
                return WIN_STATE_DRAW;
            case NO_GAME_END:
                return NO_WINNER;
            case MATED:
                return (currentPlayer == WHITE) ? BLACK_WINS : WHITE_WINS;
            default:
                assert(false);
                return NO_WINNER;
        }
    }

    inline constexpr Score scoreOf(GameEndState gameEndState) {
        switch (gameEndState) {
            case DRAW:
                return SCORE_DRAW;
            case MATED:
                return SCORE_MATED;
            default:
                assert(false);//only draw or mated can be converted to score be draw or mated
        }
    }

    inline constexpr Score operator+(Score score1, Score score2) {
        return static_cast<Score>(static_cast<int>(score1) + static_cast<int>(score2));
    }

    inline constexpr Score operator-(Score score1, Score score2) {
        return static_cast<Score>(static_cast<int>(score1) - static_cast<int>(score2));
    }

    inline constexpr Score operator+(Score score, int increase) {
        return static_cast<Score>(static_cast<int>(score) + increase);
    }

    inline constexpr Score operator-(Score score, int decrease) {
        return static_cast<Score>(static_cast<int>(score) - decrease);
    }

    inline constexpr Score operator-(Score score) {
        return static_cast<Score>(-static_cast<int>(score));
    }

    inline constexpr Score &operator+=(Score &score, int increase) {
        score = score + increase;
        return score;
    }

    inline constexpr Score &operator-=(Score &score, int decrease) {
        score = score - decrease;
        return score;
    }

    inline constexpr Score &operator++(Score &score) {
        return score += 1;
    }

    inline constexpr Score &operator--(Score &score) {
        return score -= 1;
    }

    inline constexpr Score operator*(Score score, Player player) {
        return static_cast<Score>(score * multiplierOf(player));
    }

    inline constexpr Score mateIn(int depth){
        assert(depth<MAX_DEPTH);
        return SCORE_MATE - depth;
    }

    inline constexpr Score matedIn(int depth){
        assert(depth<MAX_DEPTH);
        return SCORE_MATED + depth;
    }


    enum BoundType : uint8_t {
        BOUND_UNINITIALIZED = 0,
        BOUND_LOWER = 0b01,
        BOUND_UPPER = 0b10,
        BOUND_EXACT = BOUND_LOWER | BOUND_UPPER,
        BOUND_CURRENTLY_SEARCHING = 0b100
    };

    inline BoundType operator^(BoundType b1, BoundType b2) {
        return static_cast<BoundType>(static_cast<uint8_t>(b1) ^ b2);
    }

    inline BoundType& operator^=(BoundType& b1, BoundType b2) {
        return b1 = b1^b2;
    }



    constexpr int MAX_MOVES = 256;

    struct Indent {
        explicit Indent(int indent);

        int indent;

        friend ostream &operator<<(ostream &os, const Chess::Indent &indent);
    };

    inline int parseDigit(char digit) {
        return digit - '0';
    }
}
#endif //CHESS_TYPES_H