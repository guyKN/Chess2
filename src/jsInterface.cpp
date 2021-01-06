//
// Created by guykn on 12/14/2020.
//

#include <Search.h>
#include "Bitboards.h"
#include "iostream"
#include "ChessBoard.h"
#include "types.h"
#include "jsInterface.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
namespace WASM {
    ChessBoard chessBoard = ChessBoard();
    MoveList moveList;

    Square selectedSquare;
    MoveList movesFromSquare;

    Move prevMove;
    MoveRevertData moveRevertData;

    WinState winState;

    GameHistory gameHistory;

    Search search;

    void WASM_initData() {
        initBitboards();
    }

    int WASM_pieceOn(int square) {
        return chessBoard.getPieceOn(static_cast<Square>(square));
    }

    void WASM_calculateMoves() {
        moveList.clear();
        winState = winStateOf(chessBoard.generateMoves(moveList), chessBoard.getCurrentPlayer());
    }

    bool WASM_isLegalMoveStart(int square) {
        return chessBoard.isLegalMoveStart(static_cast<Square>(square));
    }

    void WASM_calculateMovesFrom(int square) {
        movesFromSquare.clear();
        selectedSquare = static_cast<Square>(square);
        moveList.movesFrom(selectedSquare, movesFromSquare);
    }

    bool WASM_isLegalMoveTo(int dstSquare) {
        return movesFromSquare.getMoveFromInputData(
                MoveInputData(selectedSquare, static_cast<Square>(dstSquare))).isOk();
    }

    bool WASM_doMoveIfLegal(int srcSquare, int dstSquare) {
        Move move = movesFromSquare.getMoveFromInputData(
                MoveInputData(static_cast<Square>(srcSquare), static_cast<Square>(dstSquare)));
        if (!move.isOk()) {
            return false;
        } else {
            moveRevertData = chessBoard.doMove(move);
            gameHistory.addMove(move);
            prevMove = move;
            chessBoard.assertOk();
            cout << "eval: " << chessBoard.evaluateWhite() << "\n";
            return true;
        }
    }

    void WASM_undoMove(){
        chessBoard.undoMove(prevMove, moveRevertData);
        chessBoard.assertOk();
    }

    int WASM_checkWinner() {
        return winState;
    }

    void WASM_printMoves() {
        cout << moveList;
    }

    bool WASM_currentPlayer() {
        return chessBoard.getCurrentPlayer();
    }

    void WASM_resetBoard() {
        chessBoard.resetPosition();
    }

    bool WASM_isThreatTo(Square square) {
        return chessBoard.getPinned() & maskOf(square);
    }

    void WASM_printBitboards() {
        chessBoard.printBitboards();
        cout << gameHistory;
        cout << search.gameHistory;
    }

    void WASM_doAiMove(int depth){
        search.setPos(chessBoard);
        Move move = search.bestMove(depth);
        chessBoard.doMove(move);
        gameHistory.addMove(move);
    }

    void doThing(const int& a){
        cout << a << "\n";
    }

    void WASM_runTest(){
        int a = 10;
        doThing(a);
    }

    bool WASM_gotoPos() {
        std::string fen = "8/8/8/8/1k1p1R2/8/4P3/7K w - - 0 1";
        if(chessBoard.parseFen(fen)){
            return true;
        } else{
            chessBoard = ChessBoard();
            return false;
        }
    }
}
#pragma clang diagnostic pop