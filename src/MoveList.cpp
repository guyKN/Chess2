//
// Created by guykn on 12/11/2020.
//

#include "MoveList.h"

Chess::Move Chess::MoveList::getMoveFromInputData(MoveInputData moveInput) {
    for(auto &move:moves){
        if(move.matchesMoveInput(moveInput)){
            return move;
        }
    }
    return Move::invalidMove;
}

ostream &Chess::operator<<(ostream &outStream, const Chess::MoveList &moveList) {
    for(int i=0;i<moveList.size();i++){
        Move move = moveList[i];
        outStream << move << "\n";
    }
    return outStream;
}

void Chess::MoveList::movesFrom(Chess::Square square, MoveList &moveList) {
    for(int i=0;i< size();i++){
        Move move = moves[i];
        if(move.srcSquare==square){
            moveList.addMove(move);
        }
    }

}
