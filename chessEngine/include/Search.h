//
// Created by guykn on 12/30/2020.
//

#ifndef CHESS_SEARCH_H
#define CHESS_SEARCH_H

#include "ChessBoard.h"

namespace Chess {
    class Search {

        constexpr static bool PRINT_DEBUG = false;

        GameHistory gameHistory_;

        int numNonLeafNodes = 0;
        int numLeaves = 0;
        int startingDepth = 0;

        Score bestLineScore = SCORE_ZERO;

    public:

        ChessBoard chessBoard;

        const GameHistory &gameHistory = gameHistory_;

        explicit Search(const ChessBoard &chessBoard) : chessBoard(chessBoard) {}

        Search() = default;

        inline Move bestMove(int depth) {
            numNonLeafNodes = 0;
            numLeaves = 0;
            return alphaBetaRoot(depth);
        }

        int getNumLeaves() const {
            return numLeaves;
        }

        int getNumNodes() const {
            return numNonLeafNodes + numLeaves;
        }

        inline Score getScore() const {
            return bestLineScore;
        }


        uint64_t perft(int depth);

    private:
        /// since score is mesured in terms of ply to mate from root, when we write the score in the transposition table
        /// it won't be known what the root is, so the information won't be true when loading it. So we have to convert
        /// it from ply to mate from root, to ply to mate from the current position
        inline Score toTranspositionTable(Score score, int depthLeft) const {
            if (score > SCORE_KNOWN_WIN) {
                return score + (startingDepth - depthLeft);
            } else if (score < SCORE_KNOWN_LOSS) {
                return score - (startingDepth - depthLeft);
            } else {
                return score;
            }
        }

        inline Score fromTranspositionTable(Score score, int depthLeft) const {
            if (score > SCORE_KNOWN_WIN) {
                return score - (startingDepth - depthLeft);
            } else if(score < SCORE_KNOWN_LOSS){
                return score + (startingDepth - depthLeft);
            } else{
                return score;
            }
        }

        Score alphaBeta(Score alpha, Score beta, int depthLeft);

        Move alphaBetaRoot(int depth);
    };
}


#endif //CHESS_SEARCH_H
