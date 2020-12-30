//
// Created by guykn on 12/14/2020.
//

#include "Bitboards.h"
#include "iostream"
#include "ChessBoard.h"
#include "types.h"
#include "jsInterface.h"

namespace WASM {
    ChessBoard chessBoard = ChessBoard();
    MoveList moveList;

    Square selectedSquare;
    MoveList movesFromSquare;

    Move prevMove;
    MoveRevertData moveRevertData;

    WinState winState;

    void WASM_initData() {
        initBitboards();
    }

    int WASM_pieceOn(int square) {
        return chessBoard.getPieceOn(static_cast<Square>(square));
    }

    void WASM_calculateMoves() {
        moveList.clear();
        winState = chessBoard.generateMoves(moveList);
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
            chessBoard.doGameMove(move);
            prevMove = move;
            chessBoard.assertOk();
            cout << "eval: " << chessBoard.evaluate() << "\n";
            return true;
        }
    }

    void WASM_undoMove(){
        chessBoard.undoMove(prevMove, MoveRevertData());
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
        cout << *chessBoard.getGameHistory();
    }
}