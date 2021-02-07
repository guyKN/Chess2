#include <iostream>
#include "Bitboards.h"
#include "ChessBoard.h"
#include <climits>
#include <Search.h>
#include <test.h>
#include <string>
#include <Move.h>
#include <ExchangeEvaluation.h>
#include <init.h>
#include "Benchmarks.h"
#include "Uci.h"

using namespace Chess;
using Uci::uciMain;
using std::cout;

[[noreturn]] void playGameAscii() {
    Search search;
    ChessBoard chessBoard;
    while (true) {
        chessBoard.assertOk();
        MoveChunk moveChunk{};

        ThreatData threatData{};
        chessBoard.threatData = &threatData;

        chessBoard.calculateAllThreats();
        chessBoard.generateMoves<CAPTURES>(moveChunk);
        cout << "Winning captures: \n" << moveChunk.moveList << "\n";
        moveChunk.moveList.clear();
        chessBoard.generateMoves<NON_CAPTURES>(moveChunk);
        cout << "Normal Moves: \n" << moveChunk.moveList << "\n"
        << "Losing Captures: :\n" << moveChunk.losingCaptures;

        chessBoard.generateMoves<ALL>(moveChunk);

        cout << chessBoard << "\n" << "Enter a move: ";
        MoveInputData moveInput;
        cin >> moveInput;

        Move move = moveChunk.moveList.getMoveFromInputData(moveInput);
        if (move.isOk()) {
            chessBoard.doGameMove(move);
            assert(chessBoard.isOk());
//            search.chessBoard = chessBoard;
//            Move aiMove = search.bestMove(5);
//            chessBoard.doGameMove(aiMove);
//            cout << "AI move: " << aiMove << "\n";
        } else {
            cout << "Invalid move. Please enter a move again.\n";
        }
    }
}

void test(){
    Move move = Move::normalMove(SQ_E2, SQ_E4);
    assert(move.moveType() == Move::NORMAL_MOVE);
    cout << move << "\n";
    move = Move::castle(CASTLE_BLACK_KING_SIDE);
    assert(move.moveType() == Move::CASTLING_MOVE);
    assert(move.castlingType()==CASTLE_BLACK_KING_SIDE);
    cout << move << "\n";
    move = Move::promotionMove(SQ_C7, SQ_C8, PIECE_TYPE_QUEEN);
    assert(move.moveType() == Move::PROMOTION_MOVE);

    move = Move::pawnForward2(SQ_E7, SQ_E5);
    assert(move.moveType() == Move::EN_PASSANT_MOVE);
    assert(!move.isEnPassantCapture());
    cout << move << "\n";

    move = Move::enPassantCapture(SQ_D5, SQ_C6);
    assert(move.moveType() == Move::EN_PASSANT_MOVE);
    assert(move.isEnPassantCapture());
    cout << move << "\n";

    int a[5]{};
    cout << "\n\n\n";
    cout << a[0] << "\n";
    cout << a[1]<< "\n";
    cout << a[2]<< "\n";
    cout << a[3]<< "\n";
    cout << a[4]<< "\n";
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
        } else if (firstArg=="--test") {
            test();
        } else if (firstArg == "--see") {
            initExchangeLookup();
        } else{
            cout << "Invalid arguments\n";
        }
    }
    return 0;
}
