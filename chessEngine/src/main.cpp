#include <iostream>
#include "Bitboards.h"
#include "ChessBoard.h"
#include <climits>
#include <Search.h>
#include <test.h>
#include <string>
#include "Benchmarks.h"
#include "Uci.h"

using namespace Chess;
using Uci::uciMain;
using std::cout;

[[noreturn]] void playGameAscii() {
    Search search;
    ChessBoard chessBoard;
    while (true) {
        cout << "hashKey: 0x" << std::hex << chessBoard.getHashKey() << std::dec << "\n";
        chessBoard.getFen(cout);
        chessBoard.assertOk();
        MoveList moveList;
        chessBoard.generateMoves(moveList);
        cout << chessBoard << "\n" << "Enter a move: ";
        MoveInputData moveInput = MoveInputData::readMove(cin);
        Move move = moveList.getMoveFromInputData(moveInput);
        if (move.isOk()) {
            chessBoard.doMove(move);
            cout << "eval: " << chessBoard.evaluateWhite() << "\n";
            search.chessBoard = chessBoard;
            Move aiMove = search.bestMove(4);
            cout << aiMove << "\n";
            chessBoard.doMove(aiMove);
        } else {
            cout << "Invalid move. Please enter a move again.";
        }
    }
}

int main(int argc, char *argv[]) {
#ifdef TO_WASM
    initAll();
    return 0;
#endif
    if (argc == 1) {
        // engine used as uci.
        uciMain();
        return 0;
    } else {
        string firstArg = argv[1];
        if (firstArg == "--benchmark") {
            cout << "benchmarks\n";
            doBenchmarks();
            return 0;
        } else if (firstArg == "--ascii") {
            initAll();
            playGameAscii();
            return 0;
        } else if (firstArg == "--generate-seeds" && argc >= 2) {
            string secondArg = argv[2];
            try {
                int numAttempts = std::stoi(secondArg);
                cout << "generating seeds with " << numAttempts << " attempts\n";
                generateSeeds(numAttempts);
            } catch (std::invalid_argument &e) {
                cout << "Invalid arguments\n";
            }
        } else {
            cout << "Invalid arguments\n";
        }
    }
    return 0;
}
