#include <iostream>
#include "Bitboards.h"
#include "ChessBoard.h"
#include <climits>
#include <Search.h>
#include <test.h>
#include "Benchmarks.h"
#include "Uci.h"

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
    if (argc >= 1 && std::string(argv[1]) == "--benchmark"){
        cout << "benchmarks\n";
        doBenchmarks();
        return 0;
    }
#ifdef TO_UCI
    uciMain();
#endif
#ifdef TO_COMMAND_LINE
#if GENERATE_SEEDS==0
        initBitboards();
        playGame();
#endif //GENERATE_SEEDS
#endif //TO_COMMAND_LINE
    return 0;
}
