//
// Created by guykn on 12/14/2020.
//

#include <Search.h>
#include "Bitboards.h"
#include "iostream"
#include "ChessBoard.h"
#include "types.h"
#include "WebAssembly.h"

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

    Key prevKey;

    void WASM_initData() {
        initAll();
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
            prevKey = chessBoard.getHashKey();
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
        assert(chessBoard.getHashKey() == prevKey);
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
        search.chessBoard = chessBoard;
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
        std::string fen = "r1bq1bnr/2pk1Qpp/2p1p3/p2p4/3P4/5N2/PPP2PPP/RNBQK2R w - 1 0";
        if(chessBoard.parseFen(fen)){
            return true;
        } else{
            chessBoard = ChessBoard();
            return false;
        }
    }

    bool WASM_doMoveSequence(){
        std::string moveSequence = "e2e4 b8c6 d2d4 e7e6 e4e5 d7d5 f1b5 a7a6 b5c6 b7c6 g1f3 f7f6 e5f6";
        std::stringstream stringStream{moveSequence};
        if (chessBoard.doMoves(stringStream)){
            return true;
        } else{
            return false;
        }
    }
}
#pragma clang diagnostic pop