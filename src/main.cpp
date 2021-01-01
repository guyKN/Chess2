#include <iostream>
#include "Bitboards.h"
#include "ChessBoard.h"
#include <climits>
#include <Search.h>

using namespace Chess;
using std::cout;

[[noreturn]] void playGame() {
    Search search;
    ChessBoard chessBoard;
    while (true) {
        chessBoard.assertOk();
        MoveList moveList;
        chessBoard.generateMoves(moveList);
        cout << chessBoard << "\n" << "Enter a move: ";
        MoveInputData moveInput = MoveInputData::readMove();
        Move move = moveList.getMoveFromInputData(moveInput);
        if (move.isOk()) {
            chessBoard.doMove(move);
            cout << "eval: " << chessBoard.evaluateWhite() << "\n";
            search.setPos(chessBoard);
            Move aiMove = search.bestMove(4);
            cout << aiMove << "\n";
            chessBoard.doMove(aiMove);
        } else {
            cout << "Invalid move. Please enter a move again.";
        }
    }
}

int main() {
    initBitboards();
#ifdef TO_COMMAND_LINE
#ifndef GENERATE_SEEDS
    playGame();
#endif //GENERATE_SEEDS
#endif //TO_COMMAND_LINE
    return 0;
}
