//
// Created by guykn on 12/25/2020.
//

#include "GameHistory.h"
namespace Chess {
    ostream &operator<<(ostream &os, const Chess::GameHistory &gameHistory) {
        for (int i = 0; i < gameHistory.moves.size(); i++) {
            Move move = gameHistory.moves[i];
            if (i % 2 == 0) {
                os << "\n" << (i / 2 + 1) << ". " << move;
            } else {
                os << "  " << move;
            }
        }
        os << "\n";
        return os;
    }
}