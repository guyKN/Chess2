//
// Created by guykn on 12/14/2020.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#ifndef CHESS_JSINTERFACE_H
#define CHESS_JSINTERFACE_H

#include <climits>
#include "types.h"
#include "ChessBoard.h"
#include "iostream"
#include "Bitboards.h"

using namespace Chess;
using std::cout;

namespace WASM {

    extern "C" {
    void WASM_initData();

    int WASM_pieceOn(int square);

    void WASM_calculateMoves();

    bool WASM_isLegalMoveStart(int square);

    void WASM_calculateMovesFrom(int square);

    bool WASM_isLegalMoveTo(int dstSquare);

    bool WASM_doMoveIfLegal(int srcSquare, int dstSquare);

    void WASM_undoMove();

    int WASM_checkWinner();

    void WASM_printMoves();

    bool WASM_currentPlayer();

    void WASM_resetBoard();

    bool WASM_isThreatTo(Square square);

    void WASM_printBitboards();

    bool WASM_is64bit();

    void WASM_doAiMove();

    void WASM_runTest();

    };
}


#endif //CHESS_JSINTERFACE_H
#pragma clang diagnostic pop
