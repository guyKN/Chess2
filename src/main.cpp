#include <iostream>
#include "Bitboards.h"
#include "ChessBoard.h"

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
    initLookupTables();
    Bitboard out[8];
    Bitboard in = maskOf(SQ_C3) | maskOf(SQ_E7) | maskOf(SQ_G7) | maskOfRank(RANK_2) | maskOfFile(FILE_D);
    printBitboard(in);
    Bitboard legalMoves = queenMovesFrom_slow(SQ_E5, in);
    printBitboard(legalMoves);

    return 0;
#ifdef TO_COMMAND_LINE
    playGame();
#endif //TO_COMMAND_LINE
    return 0;
}

