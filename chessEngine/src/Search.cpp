//
// Created by guykn on 12/30/2020.
//


// todo: speed up by remove unnecassary checks
#include <TransPositionTable.h>
#include "Search.h"
#include "Uci.h"
#include <ostream>

namespace Chess {

    //todo: fuilty pruning. have a threshold for max improvement per depth, and if the score physicly can't surpass that, given the number of moves, return the evaluation


    Score Search::alphaBeta(Score alpha, Score beta, int depthLeft) {
        if (depthLeft == 0) {
            numLeaves++;
            Score score = chessBoard.evaluate();
            return score;
        }

        numNonLeafNodes++;
        MoveList moveList;

        if (!chessBoard.isOk()) {
            cout << "chessBoard not OK\n";
            chessBoard.printBitboards();
            cout << gameHistory_;
            exit(987);
        }

        //check for threefold repetition
        if (repeatedPositions.count(chessBoard.getHashKey())) {
            cout << "in threefold repetition. \n";
            return SCORE_DRAW;
        }

        bool ttEntryFound;
        TransPositionTable::Entry &ttEntry = transPositionTable.probe(chessBoard.getHashKey(), ttEntryFound);
        ttEntry.onVisit(chessBoard.getMoveCount());
        Move ttBestMove = Move::invalid();
        if (ttEntryFound) {
            //todo: check if this is causing the engine to constantly blunder
            if (ttEntry.isCurrentlySearched()) {
                // repeated position, but to save time, we just a call it a draw instead of waiting for threethold repetition
                // todo: ensure this is only called in the correct times
                return SCORE_DRAW;
            }
            if (ttEntry.depth() >= depthLeft) {
                //todo: already use fromTranspositionTable() when comparing scores
                //todo: bring back upon implementing PV search

//                if (ttEntry.isExact() ||
//                    (ttEntry.isUpperBound() && ttEntry.score() <= alpha) ||
//                    (ttEntry.isLowerBound() && ttEntry.score() >= beta)) {
//                    return fromTranspositionTable(ttEntry.score(), depthLeft);
//                }
            }
            ttBestMove = ttEntry.bestMove();
            moveList.addMove(ttBestMove);
            assert(ttBestMove.isOk());
            assert(ttEntry.key() == chessBoard.getHashKey());
        }


        GameEndState gameEndState = chessBoard.generateMoves(moveList);

        switch (gameEndState) {
            case DRAW:
                //todo: handle putting data into transposition table
                return SCORE_DRAW;
            case MATED:
                return matedIn(startingDepth - depthLeft);
            case NO_GAME_END:
                break;
        }

        if (ttEntry.isCurrentlySearched()) {
            cout << "key: " << chessBoard.getHashKey() << "\n";
            Uci::error("empty ttEntry is currently being searched\n ");
            exit(713);
        }


        ttEntry.startSearching();

        if (!ttEntryFound) {
            ttEntry.setKey(chessBoard.getHashKey());
        }

        Score bestScore = -SCORE_INFINITY;
        bool skipTtMove = ttEntryFound && !moveList.notFirstContains(ttBestMove);

        if (skipTtMove) {
            Uci::log("skipping TT move Due to key conflict. ");
        }

        Move bestMove = Move::invalid();

        bool passedAlpha = false;

        for (const Move *pMove = moveList.firstMove() + (skipTtMove ? 1 : 0); pMove != moveList.lastMove(); pMove++) {
            if (pMove != moveList.firstMove() && (*pMove == ttBestMove)) {
                //since the best bestMove apears both in the movelist by natural generating by being pushed to its front, we ignore it
                continue;
            }
            gameHistory_.addMove(*pMove);
            MoveRevertData moveRevertData = chessBoard.doMove(*pMove);
            Score score = -alphaBeta(-beta, -alpha, depthLeft - 1);
            chessBoard.undoMove(*pMove, moveRevertData);

            gameHistory_.pop();
            //todo: changed order, make sure this isn't completely wrong
            if (score > bestScore) {
                bestScore = score;
                bestMove = *pMove;
                if (score > alpha) {
                    passedAlpha = true;
                    if (score < beta) {
                        alpha = score;
                    } else {
                        break;
                    }
                }
            }
        }

        ttEntry.stopSearching();
        ttEntry.setBestMove(bestMove);
        //if (bestScore <= alpha) {
        if (!passedAlpha) {
            ttEntry.setBoundType(BOUND_UPPER);
        } else if (bestScore > beta) {
            ttEntry.setBoundType(BOUND_LOWER);
        } else {
            ttEntry.setBoundType(BOUND_EXACT);
        }
        ttEntry.setScore(toTranspositionTable(bestScore, depthLeft));
        ttEntry.setDepth(depthLeft);

        if (ttEntry.key() != chessBoard.getHashKey()) {
            Uci::error("ttEntry key changed mid search. ");
            exit(171);
        }
        return bestScore;
    }

    Move Search::alphaBetaRoot(int depth) {
        startingDepth = depth;
        gameHistory_.clear();
        Score alpha = -SCORE_INFINITY;
        Score beta = SCORE_INFINITY;

        numLowerBound = 0;
        numUpperBound = 0;
        numExactBound = 0;

        Move bestMove;

        MoveList moveList;
        GameEndState gameEndState = chessBoard.generateMoves(moveList);

        assert(gameEndState == NO_GAME_END && depth > 0);

        for (const Move *pMove = moveList.firstMove(); pMove < moveList.lastMove(); pMove++) {
            gameHistory_.addMove(*pMove);
            MoveRevertData moveRevertData = chessBoard.doMove(*pMove);
            Score score = -alphaBeta(-beta, -alpha, depth - 1);
            gameHistory_.pop();
            chessBoard.undoMove(*pMove, moveRevertData);
            if (score > alpha) {
                alpha = score;
                bestMove = *pMove;
            }
        }

        bestLineScore = alpha;
        if (false) {
            if (transPositionTable.numPositionsCurrentlySearched() != 0) {
                Uci::error("positions are still being searched after search is done");
                exit(129);
            }
        }

        return bestMove;
    }

    uint64_t Search::perft(int depth) {
        MoveList moveList;
        uint64_t numNodes = 0;
        chessBoard.generateMoves(moveList);
        if (depth == 1) {
            return moveList.size();
        }
        for (const Move *move = moveList.firstMove(); move != moveList.lastMove(); ++move) {
            MoveRevertData moveRevertData = chessBoard.doMove(*move);
            numNodes += perft(depth - 1);
            chessBoard.undoMove(*move, moveRevertData);
        }
        return numNodes;
    }
}
