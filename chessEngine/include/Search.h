//
// Created by guykn on 12/30/2020.
//

#ifndef CHESS_SEARCH_H
#define CHESS_SEARCH_H

#include "ChessBoard.h"
#include <set>

namespace Chess {
    class Search {

        constexpr static bool PRINT_DEBUG = false;

        GameHistory gameHistory_;

        int numNonLeafNodes = 0;
        int numLeaves = 0;
        int startingDepth = 0;

        Score bestLineScore = SCORE_ZERO;

        int numLowerBound = 0;
        int numUpperBound = 0;
        int numExactBound = 0;
        int numPvNodes;

        std::set<Key> repeatedPositions;

        PRNG prng{123456789};

    public:

        ChessBoard chessBoard;

        const GameHistory &gameHistory = gameHistory_;

        explicit Search(const ChessBoard &chessBoard) : chessBoard(chessBoard) {}

        Search() = default;

        template<typename Timeout>
        Move bestMove(Timeout timeout) {
            repeatedPositions = chessBoard.getRepeatedPositions();
            numNonLeafNodes = 0;
            numLeaves = 0;
            numLowerBound = 0;
            numUpperBound = 0;
            numExactBound = 0;
            startingDepth = 2;
            numPvNodes = 0;
            Move bestMove;
            while (!timeout(startingDepth)) {
                bestMove = alphaBetaRoot(startingDepth);
                startingDepth++;
            }
            cout << "num PV nodes: " << numPvNodes << "\n";
            return bestMove;
        }

        Move bestMoveAtDepth(int depth) {
            repeatedPositions = chessBoard.getRepeatedPositions();
            numNonLeafNodes = 0;
            numLeaves = 0;
            numPvNodes = 0;
            startingDepth = depth;
            const Move &bestMove = alphaBetaRoot(startingDepth);
            cout << "num PV nodes: " << numPvNodes << "\n";
            return bestMove;
        }

        inline int getDepth(){
            return startingDepth;
        }

        inline int getNumLeaves() const {
            return numLeaves;
        }

        inline int getNumNodes() const {
            return numNonLeafNodes + numLeaves;
        }

        inline Score getScore() const {
            return bestLineScore;
        }


        uint64_t perft(int depth);

        void onError() const;

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
            } else if (score < SCORE_KNOWN_LOSS) {
                return score + (startingDepth - depthLeft);
            } else {
                return score;
            }
        }

        template<bool pvNode>
        Score alphaBeta(Score alpha, Score beta, int depthLeft);

        Score quiescenceSearch(Score alpha, Score beta);

        Move alphaBetaRoot(int depth);

    };
}

#endif //CHESS_SEARCH_H
