#include <iostream>
#include "Bitboards.h"
#include "ChessBoard.h"
#include <climits>
#include <Search.h>
#include <test.h>
#include "Benchmarks.h"

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
            Move aiMove = search.bestMove(5);
            cout << aiMove << "\n";
            chessBoard.doMove(aiMove);
        } else {
            cout << "Invalid move. Please enter a move again.";
        }
    }
}

int main(int argc, char *argv[]) {
    initBitboards();
    cout << "done with lookup tables\n";
    if(argc == 1) {
#ifdef TO_COMMAND_LINE
#if GENERATE_SEEDS==0
        playGame();
#endif //GENERATE_SEEDS
#endif //TO_COMMAND_LINE
    } else if(std::string(argv[1]) == "--benchmark"){
        doBenchmarks();
    }
    return 0;
}
