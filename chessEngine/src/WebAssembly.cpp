//
// Created by guykn on 12/14/2020.
//

#include <Search.h>
#include <init.h>
#include "Bitboards.h"
#include "iostream"
#include "ChessBoard.h"
#include "types.h"
#include "WebAssembly.h"
#include <MoveSelector.h>


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
using namespace Chess;
namespace WASM {

    //todo: bring back what is needed for new chessboard code.

    ChessBoard chessBoard{};
    MoveChunk moveChunk;

    Square selectedSquare;
    MoveList movesFromSquare;

    WinState winState;

    GameHistory gameHistory;

    Search search;

    void WASM_initData() {
        initAll();
    }

    int WASM_pieceOn(int square) {
        return chessBoard.getPieceOn(static_cast<Square>(square));
    }

    void WASM_calculateMoves() {
        moveChunk.moveList.clear();
        winState = chessBoard.generateThreatsAndMoves(moveChunk);
    }

    bool WASM_isLegalMoveStart(int square) {
        return chessBoard.isLegalMoveStart(static_cast<Square>(square));
    }

    void WASM_calculateMovesFrom(int square) {
        movesFromSquare.clear();
        selectedSquare = static_cast<Square>(square);
        moveChunk.moveList.movesFrom(selectedSquare, movesFromSquare);
    }

    bool WASM_isLegalMoveTo(int dstSquare) {
        MoveInputData moveInputData = {selectedSquare,
                                       static_cast<Square>(dstSquare),
                                       PIECE_TYPE_NONE,
                                       false,
                                       false};


        return movesFromSquare.getMoveFromInputData(moveInputData).isOk();
    }

    bool WASM_doMoveIfLegal(int srcSquare, int dstSquare) {

        MoveInputData moveInputData = {static_cast<Square>(srcSquare),
                                       static_cast<Square>(dstSquare),
                                       PIECE_TYPE_NONE,
                                       false,
                                       false};


        Move move = movesFromSquare.getMoveFromInputData(moveInputData);
        if (!move.isOk()) {
            return false;
        } else {
            chessBoard.doGameMove(move);
            gameHistory.addMove(move);
            chessBoard.assertOk();
            cout << std::hex << "------------------------------------------------\n";
            cout << "move order: \n";
            Move ttBestMove =
                    chessBoard.getCurrentPlayer() == WHITE ?
                    Move::pawnForward2(SQ_A2, SQ_A4) :
                    Move::pawnForward2(SQ_H7, SQ_H5);
            cout << "ttMove: " << ttBestMove << "\n";
            MoveSelector moveSelector{chessBoard, ttBestMove};
            while (Move nextMove = moveSelector.nextMove()) {
                cout << nextMove << "\n";
            }
            cout << std::dec;
            return true;
        }
    }

    void WASM_undoMove() {
        exit(11);
    }

    int WASM_checkWinner() {
        return winState;
    }

    void WASM_printMoves() {
        cout << moveChunk.moveList;
    }

    bool WASM_currentPlayer() {
        return chessBoard.getCurrentPlayer();
    }

    void WASM_resetBoard() {
        chessBoard.resetPosition();
    }

    bool WASM_isThreatTo(Square square) {
        return false;
    }

    void WASM_printBitboards() {
        chessBoard.printBitboards();
        cout << gameHistory;
        cout << search.gameHistory;
    }

    void WASM_doAiMove(int depth) {
//        search.chessBoard = chessBoard;
//        Move move = search.bestMove(depth);
//        chessBoard.doGameMove(move);
//        gameHistory.addMove(move);
    }

    void doThing(const int &a) {
        cout << a << "\n";
    }

    void WASM_runTest() {
        int a = 10;
        doThing(a);
    }

    bool WASM_gotoPos() {
        std::string fen = "r1bq1bnr/2pk1Qpp/2p1p3/p2p4/3P4/5N2/PPP2PPP/RNBQK2R w - 1 0";
        if (chessBoard.parseFen(fen)) {
            return true;
        } else {
            chessBoard = ChessBoard();
            return false;
        }
    }

    bool WASM_doMoveSequence() {
        std::string moveSequence = "e2e4 b8c6 d2d4 e7e6 e4e5 d7d5 f1b5 a7a6 b5c6 b7c6 g1f3 f7f6 e5f6";
        std::stringstream stringStream{moveSequence};
        if (chessBoard.doMoves(stringStream)) {
            return true;
        } else {
            return false;
        }
    }
}
#pragma clang diagnostic pop