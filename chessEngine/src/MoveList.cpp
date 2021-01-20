//
// Created by guykn on 12/11/2020.
//

#include "MoveList.h"

Chess::Move Chess::MoveList::getMoveFromInputData(MoveInputData moveInput) const {
    for (const Move* move = cbegin(); move < cend(); move++) {
        if (move->matchesMoveInput(moveInput)){
            return *move;
        }
    }
    return Move::invalid();
}

ostream &Chess::operator<<(ostream &outStream, const Chess::MoveList &moveList) {
    for (const Move* pMove = moveList.cbegin(); pMove < moveList.cend(); pMove++) {
        Move move = *pMove;
        cout << move << "\n";
    }
    return outStream;
}

void Chess::MoveList::movesFrom(Chess::Square square, MoveList &moveList) const {
    for (const Move* move = cbegin(); move < cend(); move++) {
        if(move->src() == square){
            moveList.addMove(*move);
        }
    }
}

bool Chess::MoveList::contains(Chess::Move move) const {
    for(const Move* pMove = cbegin(); pMove != cend(); pMove++){
        if(*pMove == move){
            return true;
        }
    }
    return false;
}

bool Chess::MoveList::notFirstContains(Chess::Move move) const {
    for(const Move* pMove = cbegin() + 1; pMove != cend(); pMove++){
        if(*pMove == move){
            return true;
        }
    }
    return false;
}
