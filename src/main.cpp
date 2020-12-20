#include <iostream>
#include "Bitboards.h"
#include "ChessBoard.h"
#include <climits>

using namespace Chess;
using std::cout;

[[noreturn]] void playGame(){
    ChessBoard chessBoard = ChessBoard();
        while (true) {
            chessBoard.assertOk();
            chessBoard.printBitboards();
            MoveList moveList;
            chessBoard.generateMoves(moveList);
            cout << "\n" << moveList << "\n" << chessBoard << "Enter a move: ";
            MoveInputData moveInput = MoveInputData::readMove();
            Move move = moveList.getMoveFromInputData(moveInput);

            if (move.isOk()) {
                chessBoard.doMove(move);
            } else {
                cout << "Invalid move. Please enter a move again.";
            }
        }
}



int main() {
    initBitboards();
    return 0;
#ifdef TO_COMMAND_LINE
    playGame();
#endif //TO_COMMAND_LINE
    return 0;
}

