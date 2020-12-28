#include <iostream>
#include "Bitboards.h"
#include "ChessBoard.h"
#include <climits>

using namespace Chess;
using std::cout;

[[noreturn]] void playGame(){
        while (true) {
            ChessBoard chessBoard;
            chessBoard.assertOk();
            MoveList moveList;
            chessBoard.generateMoves(moveList);
            cout << chessBoard << "\n" << "Enter a move: ";
            MoveInputData moveInput = MoveInputData::readMove();
            Move move = moveList.getMoveFromInputData(moveInput);
            if (move.isOk()) {
                chessBoard.doMove(move);
                cout << "eval: " << chessBoard.evaluate() << "\n";
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
