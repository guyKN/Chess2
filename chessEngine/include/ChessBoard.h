//
// Created by guykn on 12/7/2020.
//

#ifndef CHESS_CHESSBOARD_H
#define CHESS_CHESSBOARD_H


#include "types.h"
#include "OldMove.h"
#include "MoveInputData.h"
#include "MoveList.h"
#include "GameHistory.h"
#include "EvalData.h"
#include "sstream"
#include <vector>
#include <set>

using std::cout;
using std::stringstream;
using std::set;
using std::vector;
namespace Chess {

    class ZobristData {
        bool initialized = false;
        Key pieceKeys[NUM_SQUARES][NUM_NON_EMPTY_PIECES];
        Key castlingRightKeys[NUM_CASTLING_RIGHTS];
        Key enPassantFiles[NUM_FILES];
        Key blackToMove;
    public:
        //todo: check if returning is faster by refrence or value
        inline Key keyOf(Square square, Piece piece) const {
            assert(pieceOk(piece) && piece != PIECE_NONE);
            return pieceKeys[square][piece];
        }

        inline Key keyOf(CastlingRights castlingRights) const {
            assert(castlingRights >= CASTLE_RIGHTS_NONE && (castlingRights <= CASTLE_RIGHTS_ALL));
            return castlingRightKeys[castlingRights];
        }

        inline Key keyOf(File file) const {
            assert(file_ok(file));
            return enPassantFiles[file];
        }

        inline Key keyBlackToMove() const {
            return blackToMove;
        }

        void init();
    };

    extern ZobristData zobristData;

    struct MoveRevertData {
        CastlingRights castlingRights;

        Square enPassantSquare;

        Piece capturedPiece;
    };

    class ChessBoard {
        Bitboard pieceBitboards[NUM_PIECES] = {};
        Bitboard byPlayerBitboards[NUM_PLAYERS] = {};

        //todo: have threats just for pieces of the inactive player, to save memory
        // (but remember that this may harm the value of pieces for static bestMove_ evaluation)
        Bitboard threatsByPiece[NUM_NON_EMPTY_PIECES] = {};
        Bitboard threatsBypLayer[NUM_PLAYERS] = {};

        Bitboard checkEvasionSquares = BITBOARD_FULL;
        Bitboard pinned = BITBOARD_EMPTY;

        Piece piecesBySquare[NUM_SQUARES] = {};
        Player currentPlayer = WHITE;

        Square enPassantSquare = SQ_INVALID;

        bool isCheck;
        bool isDoubleCheck;

        CastlingRights castlingRights;

        Key hashKey = KEY_ZERO;

        EvalData evalData;

        vector<Key> positionsForRepetition{};

        unsigned int moveCount = 0;
        static constexpr bool DISABLE_SPECIAL_MOVES = false;

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

            hashKey ^= zobristData.keyOf(square, newPiece);
        }

        /// used when there was a piece on a square, and now a piece of the opposite color is there
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

            hashKey ^= zobristData.keyOf(square, prevPiece);
            hashKey ^= zobristData.keyOf(square, newPiece);
        }

        /// used when there previously a peice on a square, and now there isn't
        inline void removePieceFrom(Square square,
                                    Piece prevPiece, Player prevPlayer) {
            SquareMask squareMask = maskOf(square);
            pieceOn(square) = PIECE_NONE;

            bitboardOf(PIECE_NONE) |= squareMask;

            bitboardOf(prevPiece) &= ~squareMask;
            bitboardOf(prevPlayer) &= ~squareMask;

            hashKey ^= zobristData.keyOf(square, prevPiece);
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
                hashKey ^= zobristData.keyOf(square, prevPiece);
            }
            hashKey ^= zobristData.keyOf(square, newPiece);
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
                hashKey ^= zobristData.keyOf(square, newPiece);
            }
            hashKey ^= zobristData.keyOf(square, prevPiece);

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

        inline const Bitboard &getThreatsOf(Player player) const {
            return threatsBypLayer[player];
        }

        inline Bitboard &threatsOf(Piece piece) {
            assert(piece != PIECE_NONE);
            return threatsByPiece[piece];
        }

        inline const Bitboard &getThreatsOf(Piece piece) const {
            assert(piece != PIECE_NONE);
            return threatsByPiece[piece];
        }

        inline void swapPlayer() {
            currentPlayer = ~currentPlayer;
            hashKey ^= zobristData.keyBlackToMove();
        }


        template<Player player>
        bool mayCastleKingSide() const {
            if constexpr (player == WHITE) {
                return castlingRights & CASTLE_RIGHTS_WHITE_KING_SIDE;
            } else {
                return castlingRights & CASTLE_RIGHTS_BLACK_KING_SIDE;
            }
        }

        template<Player player>
        bool mayCastleQueenSide() const {
            if constexpr (player == WHITE) {
                return castlingRights & CASTLE_RIGHTS_WHITE_QUEEN_SIDE;
            } else {
                return castlingRights & CASTLE_RIGHTS_BLACK_QUEEN_SIDE;
            }
        }

        inline MoveRevertData getMoveRevertData(Piece capturedPiece) {
            return MoveRevertData{castlingRights, enPassantSquare, capturedPiece};
        }

        /// returns true if the move is a pawn advance or a capture
        inline bool isIreversible(Move move) const {
            return (pieceTypeOf(getPieceOn(move.src())) == PIECE_TYPE_PAWN) ||
                   (getPieceOn(move.dst()) != PIECE_NONE) || move.moveType() == Move::CASTLING_MOVE;
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

        inline ThreatMap makeThreatMap(SquareMask squareMask) const;

        bool noPieceOverlap() const;

        bool noColorOverlap() const;

        void updateCastling(const Move &move);

        void updateBitboards();

        void updateBitboardsForPlayer(Player player);

        void revertTo(const MoveRevertData &moveRevertData);

        void initHashKey();

        template<Player player>
        void calculateWiningCaptures();

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

        inline Key getHashKey() {
            return hashKey;
        }

        void setPosition(const Piece piecesArray[NUM_SQUARES], Player currentPlayer);

        void setGameHistory(GameHistory &gameHistory);

        bool doMoves(vector<MoveInputData> &moves);

        bool doMoves(std::stringstream &moveStream);

        inline Player getCurrentPlayer() {
            return currentPlayer;
        }

        inline bool getIsCheck() {
            return isCheck;
        }

        inline Bitboard getBitboardOf(Piece piece) {
            return bitboardOf(piece);
        }

        inline Bitboard getPinned() const {
            return pinned;
        }

        inline unsigned int getMoveCount() const {
            return moveCount;
        }

        const static Piece startingBoard[NUM_SQUARES];

        void printBitboards() const;

        inline bool isLegalMoveStart(Square square) {
            Piece piece = getPieceOn(square);
            return (piece != PIECE_NONE) && (playerOf(piece) == currentPlayer);
        }

        MoveRevertData doMove(Move move);

        MoveRevertData doGameMove(Move move);

        void undoMove(Move move, MoveRevertData &moveRevertData);

        inline const Piece &getPieceOn(Square square) const {
            return piecesBySquare[square];
        }

        inline Bitboard getThreats() {
            return threatsOf(~currentPlayer);
        }

        inline Bitboard getCheckEvasions() const {
            return checkEvasionSquares;
        }

        friend std::ostream &operator<<(std::ostream &os, const ChessBoard &chessBoard);

        void assertOk() const;

        GameEndState generateMoves(MoveList &moveList);

        Score evaluateWhite();

        inline Score evaluate() {
            return evaluateWhite() * currentPlayer;
        }

        bool piecesMatchArray() const;

        bool playerColorsMatch() const;

        bool isOk() const;

        bool samePositionAs(const ChessBoard &other) const;

        bool parseFen(const string &fenString);

        ostream &getFen(ostream &outputStream) const;

        set<Key> getRepeatedPositions();
    };
}


#endif //CHESS_CHESSBOARD_H
