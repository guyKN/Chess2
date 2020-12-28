//
// Created by guykn on 12/11/2020.
//

#include "MoveList.h"

Chess::Move Chess::MoveList::getMoveFromInputData(MoveInputData moveInput) const {
    for (const Move* move = firstMove(); move < lastMove(); move++) {
        if (move->matchesMoveInput(moveInput)){
            return *move;
        }
    }
    return Move::invalidMove;
}

ostream &Chess::operator<<(ostream &outStream, const Chess::MoveList &moveList) {
    for (const Move* pMove = moveList.firstMove(); pMove < moveList.lastMove(); pMove++) {
        Move move = *pMove;
        cout << move << "\n";
    }
    return outStream;
}

void Chess::MoveList::movesFrom(Chess::Square square, MoveList &moveList) const {
    for (const Move* move = firstMove(); move < lastMove(); move++) {
        if(move->srcSquare == square){
            moveList.addMove(*move);
        }
    }
}
