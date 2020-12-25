//#pragma clang diagnostic push
//
// Created by guykn on 12/7/2020.
//

#include <iostream>
#include "types.h"
# include <cassert>

using namespace Chess;
namespace Chess {
    std::ostream &printBitboard(Bitboard bitboard, std::ostream &os) {
        SquareMask squareMask = SQUARE_MASK_FIRST;
        os << "\n";
        for (Rank rank = RANK_FIRST; rank <= RANK_LAST; ++rank) {
            for (File file = FILE_FIRST; file <= FILE_LAST; ++file) {
                os << (squareMask & bitboard ? '1' : '.') << ' ';
                squareMask <<= 1;
            }
            os << "            " << rank << std::endl;
        }
        return os;
    }

    Rank rankOf(Square square) {
        return static_cast<Rank>(square / 8);
    }

    inline constexpr File fileOf(Square square) {
        return static_cast<File>(square % 8);
    }


    char toChar(Rank rank) {
        if (rank >= RANK_FIRST && rank <= RANK_LAST) {
            return RANK_NAMES[rank];
        } else {
            return '?';
        }

    }

    Rank parseRank(char rankChar) {
        for (Rank rank = RANK_FIRST; rank <= RANK_LAST; ++rank) {
            if (toChar(rank) == rankChar) {
                return rank;
            }
        }
        return RANK_INVALID;
    }

    char toChar(File file) {
        if (file >= FILE_FIRST && file <= FILE_LAST) {
            return FILE_NAMES[file];
        } else {
            return '?';
        }
    }

    File parseFile(char fileChar) {
        for (File file = FILE_FIRST; file <= FILE_LAST; ++file) {
            if (toupper(toChar(file)) == toupper(fileChar)) {
                return file;
            }
        }
        return FILE_INVALID;

    }

    Square parseSquare(std::string &str) {
        str = removeSpaces(str);
        if (str.length() != 2) {
            return SQ_INVALID;
        }
        File file = parseFile(str[0]);
        Rank rank = parseRank(str[1]);
        if (rank_ok(rank) && file_ok(file)) {
            return makeSquare(rank, file);
        } else {
            return SQ_INVALID;
        }
    }

    std::string removeSpaces(std::string &input) {
        input.erase(std::remove(input.begin(), input.end(), ' '), input.end());
        input.erase(std::remove(input.begin(), input.end(), '\n'), input.end());

        return input;
    }

    std::string toString(Square square) {
        if (!square_ok(square)) {
            return "INVALID";
        }
        std::string result = "";
        result.push_back(toChar(fileOf(square)));
        result.push_back(toChar(rankOf(square)));
        return result;
    }

    bool squareMask_ok(SquareMask squareMask) {
        SquareMask compareMask = SQUARE_MASK_FIRST;
        for (Square square = SQ_FIRST; square <= SQ_LAST; ++square) {
            if (compareMask == squareMask) {
                return true;
            }
            compareMask <<= 1;
        }
        return false;
    }

    bool notSquareMask_ok(NotSquareMask notSquareMask) {
        return squareMask_ok(~notSquareMask);
    }

    char toChar(Piece piece) {
        char pieceToChar[] = "PNBRQKpnbrqk ";
        assert(piece_ok(piece));
        return pieceToChar[piece];
    }

    Piece parsePiece(char pieceChar) {
        for(Piece piece = PIECE_FIRST;piece<=PIECE_LAST;++piece){
            if(toChar(piece) == pieceChar){
                return piece;
            }
        }
        return PIECE_INVALID;
    }

    std::ostream &printArray(Bitboard *array, int length, ostream &outputStream) {
        outputStream << '{';
        for(int i=0;i<length;i++){
            outputStream << std::hex << "0x" << array[i] << ", ";
        }
        outputStream << '}' << std::dec;
        return outputStream;
    }

    char toChar(PieceType pieceType) {
        char pieceTypeToChar[] = " NBRQK";
        return pieceTypeToChar[pieceType];
    }

    std::ostream &operator<<(ostream &outputStream, Player player) {
        return outputStream << (player == WHITE ? "White" : "Black");
    }

}