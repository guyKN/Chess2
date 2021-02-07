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
#include "ExchangeEvaluation.h"
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

    enum MoveGenType {
        ALL,
        CAPTURES, // note: promotions are also included as captures
        NON_CAPTURES
    };

    std::ostream &operator<<(ostream &os, MoveGenType moveGenType);

    struct ThreatData {
    private:
        Bitboard threatsByPiece[NUM_NON_EMPTY_PIECES]{};
        Bitboard threatsByPlayer[NUM_PLAYERS]{};
        bool _isInCheck[NUM_PLAYERS]{};
        bool _isInDoubleCheck[NUM_PLAYERS]{};
        Bitboard kingBlockers[NUM_PLAYERS]{};  // all pieces of both colors that block the king of a certain player
        // used for discovery checks and pins
    public:

        Bitboard checkEvasionSquares = BITBOARD_FULL;

        inline ThreatData() = default;

        inline bool isEmpty();

        ThreatData(const ThreatData &) = delete;

        ThreatData(const ThreatData &&) = delete;

        ThreatData &operator=(const ThreatData &) = delete;

        ThreatData &operator=(const ThreatData &&) = delete;

        ThreatMap makeThreatMap(SquareMask squareMask) const;

        inline Bitboard &threatsOf(Player player) {
            assert(this != nullptr);
            assert(isOk(player));
            return threatsByPlayer[player];
        }

        inline const Bitboard &threatsOf(Player player) const {
            assert(this != nullptr);
            assert(isOk(player));
            return threatsByPlayer[player];
        }

        inline Bitboard &threatsOf(Piece piece) {
            assert(this != nullptr);
            assert(pieceOk(piece) && piece != PIECE_NONE);
            return threatsByPiece[piece];
        }

        inline const Bitboard &threatsOf(Piece piece) const {
            assert(this != nullptr);
            assert(pieceOk(piece) && piece != PIECE_NONE);
            return threatsByPiece[piece];
        }

        inline bool &isInCheck(Player player) {
            assert(this != nullptr);
            assert(isOk(player));
            return _isInCheck[player];
        }

        inline const bool &isInCheck(Player player) const {
            assert(this != nullptr);
            assert(isOk(player));
            return _isInCheck[player];
        }

        inline bool &isInDoubleCheck(Player player) {
            assert(this != nullptr);
            assert(isOk(player));
            return _isInDoubleCheck[player];
        }

        inline const bool &isInDoubleCheck(Player player) const {
            assert(this != nullptr);
            assert(isOk(player));
            return _isInDoubleCheck[player];
        }

        inline Bitboard &kingBlockersOf(Player player) {
            assert(this != nullptr);
            assert(isOk(player));
            return kingBlockers[player];
        }

        inline const Bitboard &kingBlockersOf(Player player) const {
            assert(this != nullptr);
            assert(isOk(player));
            return kingBlockers[player];
        }

        friend std::ostream &operator<<(std::ostream &os, ThreatData &threatData);


    };

    class ChessBoard {
        Bitboard pieceBitboards[NUM_PIECES] = {};
        Bitboard byPlayerBitboards[NUM_PLAYERS] = {};
        Piece piecesBySquare[NUM_SQUARES] = {};
        Player currentPlayer = WHITE;
        CastlingRights castlingRights;
        Square enPassantSquare = SQ_INVALID;

        Key hashKey = KEY_ZERO;

        EvalData evalData;

        vector<Key> positionsForRepetition{};

        unsigned int moveCount = 0;
    public:
        ThreatData *threatData = nullptr;
    private:
        inline Bitboard &bitboardOf(Piece piece) {
            return pieceBitboards[piece];
        }

        inline Bitboard &bitboardOf(Player player) {
            return byPlayerBitboards[player];
        }

        inline const Bitboard &bitboardOf(Piece piece) const {
            return pieceBitboards[piece];
        }

        inline const Bitboard &bitboardOf(Player player) const {
            return byPlayerBitboards[player];
        }

        inline Piece &pieceOn(Square square) {
            return piecesBySquare[square];
        }

    public:
        inline const Piece &getPieceOn(Square square) const {
            return piecesBySquare[square];
        }

    private:

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

        template<Player player, MoveGenType moveGenType>
        void generateAllPieces(MoveChunk &moveChunk, Bitboard source, Bitboard target) const;

        template<Player player, MoveGenType moveGenType>
        void generatePawnMoves(MoveChunk &moveChunk, Bitboard source, Bitboard target) const;

        template<Player player, MoveGenType moveGenType>
        void generateKnightMoves(MoveChunk &moveChunk, Bitboard source, Bitboard target) const;

        template<Player player, PieceType pieceType, MoveGenType moveGenType>
        void generateSlidingPieceMoves(MoveChunk &moveChunk, Bitboard source, Bitboard target) const;

        template<Player player, MoveGenType moveGenType>
        void generateKingMoves(MoveChunk &moveChunk, Bitboard target) const;

        template<Player player, MoveGenType moveGenType1>
        void generateMovesForPlayer(MoveChunk &moveChunk) const;

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

        inline StaticEvalScore evalCapture(Square dstSquare, Piece srcPiece) const {
            Piece dstPiece = getPieceOn(dstSquare);
            ThreatMap threatMap = threatData->makeThreatMap(maskOf(dstSquare));
            assert(threatMap & threatMapOf(srcPiece));
            return staticPieceValue(dstPiece) - staticExchangeEval(threatMap ^ threatMapOf(srcPiece), srcPiece);
        }

        bool noPieceOverlap() const;

        bool noColorOverlap() const;

        void updateCastling(const Move &move);

        void updateBitboards();

        void updateBitboardsForPlayer(Player player);

        void revertTo(const MoveRevertData &moveRevertData);

        void initHashKey();

        template<Player player, MoveGenType moveGenType>
        inline Bitboard targetForMoveGenType() const {
            if constexpr (moveGenType == ALL) {
                return BITBOARD_FULL;
            } else if constexpr (moveGenType == CAPTURES) {
                return bitboardOf(~player);
            } else if constexpr (moveGenType == NON_CAPTURES) {
                return ~bitboardOf(~player);
            }
        }

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

        inline bool isInCheck() {
            return threatData->isInCheck(currentPlayer);
        }

        void setPosition(const Piece piecesArray[NUM_SQUARES], Player currentPlayer);

        void setGameHistory(GameHistory &gameHistory);

        bool doMoves(vector<MoveInputData> &moves);

        bool doMoves(std::stringstream &moveStream);

        inline Player getCurrentPlayer() {
            return currentPlayer;
        }

        inline unsigned int getMoveCount() const {
            return moveCount;
        }

        inline bool isPartiallyLegal(Move move) const {
            // when looking at moves from the transposition table, it's possible for a key collision which would cause an illegal
            // which can create very wired effects on the game
            // this function doesn't fully check move legality completely, but it checks it enough to eliminate moves that would cause major errors.
            Piece srcPiece = getPieceOn(move.src());
            Piece dstPiece = getPieceOn(move.dst());
            return (srcPiece != PIECE_NONE) && (playerOf(srcPiece) == currentPlayer) &&
                   (dstPiece == PIECE_NONE || playerOf(dstPiece) != currentPlayer);
        }

        const static Piece startingBoard[NUM_SQUARES];

        void printBitboards() const;

        inline bool isLegalMoveStart(Square square) {
            Piece piece = pieceOn(square);
            return (piece != PIECE_NONE) && (playerOf(piece) == currentPlayer);
        }

        MoveRevertData doMove(Move move);

        MoveRevertData doGameMove(Move move);

        void undoMove(Move move, MoveRevertData &moveRevertData);

        friend std::ostream &operator<<(std::ostream &os, const ChessBoard &chessBoard);

        void assertOk() const;

        template<MoveGenType moveGenType>
        void generateMoves(MoveChunk &moveChunk) const;

        WinState generateThreatsAndMoves(MoveChunk &moveChunk) {
            ThreatData threatData{};
            this->threatData = &threatData;
            calculateAllThreats();
            generateMoves<ALL>(moveChunk);
            if(moveChunk.moveList.isEmpty()){
                if (this->threatData->isInCheck(currentPlayer)){
                    return playerLoses(currentPlayer);
                } else{
                    return WIN_STATE_DRAW;
                }
            } else{
                return NO_WINNER;
            }
        }

        void calculateAllThreats();

        inline bool checksOk() {
            return !(threatData->isInCheck(~currentPlayer) ||
                     threatData->isInDoubleCheck(~currentPlayer));
        }

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

        template<MoveGenType moveGenType>
        void printMoves(){
            ThreatData threatData{};
            this->threatData = &threatData;
            calculateAllThreats();
            MoveChunk moveChunk{};
            generateMoves<moveGenType>(moveChunk);
            cout << moveChunk.moveList;
            cout << moveChunk.losingCaptures;
        }
    };
}


#endif //CHESS_CHESSBOARD_H
