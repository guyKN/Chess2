//
// Created by guykn on 12/7/2020.
//

#ifndef CHESS_CHESSBOARD_H
#define CHESS_CHESSBOARD_H


#include "types.h"
#include "Move.h"
#include "MoveInputData.h"
#include "MoveList.h"
#include "GameHistory.h"
#include "EvalData.h"

using std::cout;
namespace Chess {

    struct MoveRevertData {
        bool whiteMayCastleKingSide;
        bool whiteMayCastleQueenSide;
        bool blackMayCastleKingSide;
        bool blackMayCastleQueenSide;
    };

    //todo: expirement with additional representations in addition to bitboards
    // specificly: pieceLists as vectors, and an array of all pieces by squarew

    class ChessBoard {
        Bitboard pieceBitboards[NUM_PIECES] = {};
        Bitboard byPlayerBitboards[NUM_PLAYERS] = {};

        //todo: have threats just for pieces of the inactive player, to save memory
        // (but remember that this may harm the value of pieces for static move evaluation)
        Bitboard threatsByPiece[NUM_NON_EMPTY_PIECES] = {};
        Bitboard threatsBypLayer[NUM_PLAYERS] = {};

        Bitboard checkEvasionSquares = BITBOARD_FULL;
        Bitboard pinned = BITBOARD_EMPTY;

        Piece piecesBySquare[NUM_SQUARES] = {};
        Player currentPlayer = WHITE;

        Square enPassantSquare = SQ_INVALID;

        bool isCheck;
        bool isDoubleCheck;

        bool whiteMayCastleKingSide;
        bool whiteMayCastleQueenSide;
        bool blackMayCastleKingSide;
        bool blackMayCastleQueenSide;

        EvalData evalData;

        static constexpr bool DISABLE_SPECIAL_MOVES = true;

        inline Bitboard &bitboardOf(Piece piece) {
            return pieceBitboards[piece];
        }

        inline Bitboard &bitboardOf(Player player) {
            return byPlayerBitboards[player];
        }

        inline Piece &pieceOn(Square square) {
            return piecesBySquare[square];
        }


        /// used when there wasn't previously a piece on a square, and now there needs to be one
        inline void placePieceOn(Square square,
                                 Piece newPiece, Player newPlayer) {
            SquareMask squareMask = maskOf(square);
            pieceOn(square) = newPiece;
            bitboardOf(newPiece) |= squareMask;
            bitboardOf(newPlayer) |= squareMask;

            bitboardOf(PIECE_NONE) &= ~squareMask;
        }

        /// used when there was a piece on a square, and now the opposite piece is there
        inline void changePieceOn(Square square,
                                  Piece prevPiece,
                                  Piece newPiece, Player newPlayer) {
            SquareMask squareMask = maskOf(square);
            assert(pieceOk(newPiece) && newPiece != PIECE_NONE);
            pieceOn(square) = newPiece;

            bitboardOf(newPiece) |= squareMask;
            bitboardOf(newPlayer) |= squareMask;

            bitboardOf(prevPiece) &= ~squareMask;
            bitboardOf(~newPlayer) &= ~squareMask;
        }

        /// used when there previously a peice on a square, and now there isn't
        inline void removePieceFrom(Square square,
                                    Piece prevPiece, Player prevPlayer) {
            SquareMask squareMask = maskOf(square);
            pieceOn(square) = PIECE_NONE;

            bitboardOf(PIECE_NONE) |= squareMask;

            bitboardOf(prevPiece) &= ~squareMask;
            bitboardOf(prevPlayer) &= ~squareMask;
        }

        /// used the same as changePieceOn, but only when prevPiece may or may not be PIECE_NONE
        /// this assumes that the player of the piece being removed is the opposite player of the piece being placed
        inline void setPieceOn(Square square,
                               Piece prevPiece,
                               Piece newPiece, Player newPlayer) {
            SquareMask squareMask = maskOf(square);
            pieceOn(square) = newPiece;

            bitboardOf(newPiece) |= squareMask;
            bitboardOf(newPlayer) |= squareMask;

            bitboardOf(prevPiece) &= ~squareMask;
            if (prevPiece != PIECE_NONE) {
                bitboardOf(~newPlayer) &= ~squareMask;
            }
        }

        /// same as setPieceOn, except that prevPiece must not be pieceNone, but newPiece may or may not be
        inline void setPieceOn2(Square square,
                                Piece prevPiece, Player prevPlayer,
                                Piece newPiece) {
            SquareMask squareMask = maskOf(square);
            pieceOn(square) = newPiece;

            bitboardOf(newPiece) |= squareMask;
            if (newPiece != PIECE_NONE) {
                bitboardOf(~prevPlayer) |= squareMask;
            }

            bitboardOf(prevPiece) &= ~squareMask;
            bitboardOf(prevPlayer) &= ~squareMask;
        }


        inline Bitboard getBitboardOf(Piece piece) const {
            return pieceBitboards[piece];
        }

        inline Bitboard getBitboardOf(Player player) const {
            return byPlayerBitboards[player];
        }

        inline Bitboard &threatsOf(Player player) {
            return threatsBypLayer[player];
        }

        inline Bitboard &threatsOf(Piece piece) {
            assert(piece != PIECE_NONE);
            return threatsByPiece[piece];
        }

        inline void swapPlayer() {
            currentPlayer = ~currentPlayer;
        }


        template<Player player>
        bool mayCastleKingSide() {
            if constexpr (player == WHITE) {
                return whiteMayCastleKingSide;
            } else {
                return blackMayCastleKingSide;
            }
        }

        template<Player player>
        bool mayCastleQueenSide() {
            if constexpr (player == WHITE) {
                return whiteMayCastleQueenSide;
            } else {
                return blackMayCastleQueenSide;
            }
        }

        inline MoveRevertData getMoveRevertData() {
            return MoveRevertData{whiteMayCastleKingSide, whiteMayCastleQueenSide,
                                  blackMayCastleKingSide, blackMayCastleQueenSide};
        }

        template<Player player>
        void generateAllPieces(MoveList &moveList, Bitboard source, Bitboard target);

        template<Player player>
        void generatePawnMoves(MoveList &moveList, Bitboard source, Bitboard target);

        template<Player player>
        void generateKnightMoves(MoveList &moveList, Bitboard source, Bitboard target);

        template<Player player, PieceType pieceType>
        void generateSlidingPieceMoves(MoveList &moveList, Bitboard source, Bitboard target);

        template<Player player>
        void generateKingMoves(MoveList &moveList);

        template<Player player>
        GameEndState generateMovesForPlayer(MoveList &moveList);

        template<Player player>
        void calculateThreats();

        template<Player player>
        void calculatePawnThreats();

        template<Player player>
        void calculateKnightThreats();

        template<Player player, PieceType pieceType>
        void calculateSlidingPieceThreats();

        template<Player player>
        void calculateQueenThreats();

        template<Player player>
        void calculateKingThreats();

        bool noPieceOverlap() const;

        bool noColorOverlap() const;

        void updateCastling(const Move &move);

    public:

        inline ChessBoard(const Piece piecesBySquare[NUM_SQUARES], Player currentPlayerColor) :
                currentPlayer(currentPlayerColor) {
            evalData = EvalData();
            setPosition(piecesBySquare, currentPlayerColor);
        }

        inline ChessBoard() {
            resetPosition();
        }


        ChessBoard &operator=(const ChessBoard &other);

        inline ChessBoard(ChessBoard const &other) {
            *this = other;
        }

        inline void resetPosition() {
            setPosition(startingBoard, STARTING_PLAYER);
        }

        void setPosition(const Piece piecesArray[NUM_SQUARES], Player currentPlayer);

        inline Player getCurrentPlayer() {
            return currentPlayer;
        }

        inline bool getIsCheck() {
            return isCheck;
        }

        inline Bitboard getBitboardOf(Piece piece) {
            return bitboardOf(piece);
        }

        inline Bitboard getPinned() {
            return pinned;
        }

        const static Piece startingBoard[NUM_SQUARES];

        void printBitboards() const;

        inline bool isLegalMoveStart(Square square) {
            Piece piece = getPieceOn(square);
            return (piece != PIECE_NONE) && (playerOf(piece) == currentPlayer);
        }

        MoveRevertData doMove(const Move &move);

        void undoMove(Move &move, MoveRevertData &moveRevertData);

        void doMoveOld(Move &move);

        inline Piece getPieceOn(Square square) const {
            return piecesBySquare[square];
        }

        inline Bitboard getThreats() {
            return threatsOf(~currentPlayer);
        }

        inline Bitboard getCheckEvasions() {
            return checkEvasionSquares;
        }

        friend std::ostream &operator<<(std::ostream &os, const ChessBoard &chessBoard);

        void assertOk() const;

        GameEndState generateMoves(MoveList &moveList);

        Score evaluateWhite();

        inline Score evaluate() {
            return evaluateWhite() * currentPlayer;
        }

        void revertTo(const MoveRevertData &moveRevertData);

        bool piecesMatchArray() const;

        bool playerColorsMatch() const;

        bool isOk() const;

        bool samePositionAs(const ChessBoard &other);

        bool parseFen(std::string &fenString) {

            whiteMayCastleQueenSide = false;
            blackMayCastleQueenSide = false;
            whiteMayCastleKingSide = false;
            blackMayCastleKingSide = false;

            File file = FILE_FIRST;
            Rank rank = RANK_LAST;
            int i=0;
            while (true) {
                if(i==fenString.size()){
                    cout << "return false #1\n";
                    return false;
                }
                char c = fenString[i];
                i++;
                switch (c) {
                    case ' ':
                        break;
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8': {
                        int num = parseInt(c);
                        for (int _ = 0; _ < num; _++) {
                            pieceOn(makeSquare(rank, file)) = PIECE_NONE;
                            ++file;
                            if (!file_ok(file)){
                                cout << "return false #2\n";
                                return false;
                            }
                        }
                        break;
                    }
                    case 'P':
                    case 'N':
                    case 'B':
                    case 'R':
                    case 'Q':
                    case 'K':
                    case 'p':
                    case 'n':
                    case 'b':
                    case 'r':
                    case 'q':
                    case 'k': {
                        Piece piece = parsePiece(c);
                        pieceOn(makeSquare(rank, file)) = piece;
                        ++file;
                        if (!file_ok(file)){
                            return false;
                        }
                    }
                    case '/': {
                        if (file != FILE_LAST){
                            return false;
                        }
                        --rank;
                        file = FILE_FIRST;
                    }
                    default:
                        //invalid character
                        cout << "return false #4\n";
                        return false;
                }
                if (rank == RANK_FIRST && file == FILE_LAST){
                    break;
                }
            }

            while (true){
                if(i==fenString.size()){
                    cout << "return false #3\n";
                    return false;
                }
                char c = fenString[i];
                i++;
                switch (c) {
                    case ' ':
                        break;
                    case 'w':
                    case 'W':
                        currentPlayer = WHITE;
                        goto endOfLoop;
                    case 'b':
                    case 'B':
                        currentPlayer = BLACK;
                        goto endOfLoop;
                    default:
                        // illegal character
                        cout << "return false #5\n";
                        return false;
                }
            }

            endOfLoop:

            bool atLeastOneCastling;
            while (true){
                if(i==fenString.size()){
                    return false;
                }
                char c = fenString[i];
                switch (c) {
                    case ' ':
                        break;
                    case 'K':
                        if (whiteMayCastleKingSide){
                            // duplicate of the same castling
                            return false;
                        }
                        whiteMayCastleKingSide = true;
                        atLeastOneCastling = true;
                        break;
                    case 'Q':
                        if (whiteMayCastleQueenSide){
                            // duplicate of the same castling
                            return false;
                        }
                        whiteMayCastleQueenSide = true;
                        atLeastOneCastling = true;
                        break;
                    case 'k':
                        if (blackMayCastleKingSide){
                            // duplicate of the same castling
                            return false;
                        }
                        blackMayCastleKingSide = true;
                        atLeastOneCastling = true;
                        break;
                    case 'q':
                        if (blackMayCastleQueenSide){
                            // duplicate of the same castling
                            return false;
                        }
                        blackMayCastleQueenSide = true;
                        atLeastOneCastling = true;
                        break;
                    case '-':
                        if (!atLeastOneCastling) {
                            i++;
                        }
                        goto endOfLoop2;

                    case 'a':
                    case 'b':
                    case 'c':
                    case 'd':
                    case 'e':
                    case 'f':
                    case 'g':
                    case 'h':
                    case 'A':
                    case 'B':
                    case 'C':
                    case 'D':
                    case 'E':
                    case 'F':
                    case 'G':
                    case 'H':
                        //does not increment i
                        goto endOfLoop2;

                    default:
                        //illegal character
                        assert(false);
                }
                i++;
            }
            endOfLoop2:

            Rank enPassantRank = RANK_INVALID;
            File enPassantFile = FILE_INVALID;

            while (true){
                if(i==fenString.size()){
                    return false;
                }
                char c = fenString[i];
                i++;
                switch (c) {
                    case ' ':
                        break;
                    case 'a':
                    case 'b':
                    case 'c':
                    case 'd':
                    case 'e':
                    case 'f':
                    case 'g':
                    case 'h':
                    case 'A':
                    case 'B':
                    case 'C':
                    case 'D':
                    case 'E':
                    case 'F':
                    case 'G':
                    case 'H':
                        if(! ((enPassantFile == FILE_INVALID) && (enPassantRank == RANK_INVALID))){
                            return false;
                        }
                        enPassantFile = parseFile(c);
                        break;
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                        if(!((enPassantFile != FILE_INVALID) && (enPassantRank == RANK_INVALID))){
                            return false;
                        }
                        enPassantRank = parseRank(c);
                        enPassantSquare = makeSquare(enPassantRank, enPassantFile);
                        return true;
                    case '-':
                        enPassantSquare = SQ_INVALID;
                        return true;
                    default:
                        assert(false);
                }
            }

        }
    };
}




#endif //CHESS_CHESSBOARD_H
