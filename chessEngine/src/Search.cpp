//
// Created by guykn on 12/30/2020.
//


// todo: speed up by remove unnecassary checks
#include <TransPositionTable.h>
#include "Search.h"
#include "Uci.h"
#include <ostream>
#include <MoveSelector.h>

namespace Chess {

    //todo: fuilty pruning. have a threshold for max improvement per depth, and if the score physicly can't surpass that, given the number of moves, return the evaluation


    Score Search::alphaBeta(Score alpha, Score beta, int depthLeft) {
        if (depthLeft == 0) {
            return quiescenceSearch(alpha, beta);
            //return chessBoard.evaluate();
        }

        numNonLeafNodes++;

        if (!chessBoard.isOk()) {
            cout << "chessBoard not OK\n";
            onError();
        }

        //check for threefold repetition
        if (repeatedPositions.count(chessBoard.getHashKey())) {
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
            }

            ttBestMove = ttEntry.bestMove();
            if(ttBestMove == Move::invalid()){
                // the position is checkmate or stalemate. return the score from it.
                return fromTranspositionTable(ttEntry.score(), depthLeft);
            }
            if(!ttBestMove.isOk()){
                cout << ttBestMove;
                cout << "ttBestMove not Ok";
                onError();
            }
            assert(ttEntry.key() == chessBoard.getHashKey());
        } else {
            ttEntry.setKey(chessBoard.getHashKey());
        }

        if (ttEntry.isCurrentlySearched()) {
            cout << "key: " << chessBoard.getHashKey() << "\n";
            Uci::error("empty ttEntry is currently being searched\n ");
            onError();
        }

        ttEntry.startSearching();

        Score bestScore = -SCORE_INFINITY;
        MoveSelector moveSelector{chessBoard, true, ttBestMove, this};

        Move bestMove = Move::invalid();

        bool passedAlpha = false;
        bool noLegalMoves = true;
        while (Move move = moveSelector.nextMove()) {
            noLegalMoves = false;
            gameHistory_.addMove(move);
            MoveRevertData moveRevertData = chessBoard.doMove(move);
            Score score = -alphaBeta(-beta, -alpha, depthLeft - 1);
            chessBoard.undoMove(move, moveRevertData);

            gameHistory_.pop();
            //todo: changed order, make sure this isn't completely wrong
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
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
        if (noLegalMoves) {
            if (chessBoard.isInCheck()) {
                ttEntry.setScore(SCORE_MATED);
                ttEntry.setBoundType(BOUND_EXACT);
                ttEntry.setDepth(0);
                ttEntry.setBestMove(Move::invalid());
                return matedIn(depthLeft);
            } else {
                ttEntry.setScore(SCORE_DRAW);
                ttEntry.setBoundType(BOUND_EXACT);
                ttEntry.setDepth(0);
                ttEntry.setBestMove(Move::invalid());
                return SCORE_DRAW;
            }
        }
        if(!bestMove.isOk()){
            cout << "bestMove is found as not OK!";
            onError();
        }
        ttEntry.setBestMove(bestMove);
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
            onError();
        }
        return bestScore;
    }

    Score Search::quiescenceSearch(Score alpha, Score beta) {
        numLeaves++;

        if (!chessBoard.isOk()) {
            cout << "chessBoard not OK\n";
            onError();
        }

        //check for threefold repetition
        if (repeatedPositions.count(chessBoard.getHashKey())) {
            return SCORE_DRAW;
        }

        Score currentScore = chessBoard.evaluate();
        if (currentScore > alpha) {
            alpha = currentScore;
        }
        if (alpha > beta) {
            return alpha;
        }

        MoveSelector moveSelector{chessBoard, false, Move::invalid(), this};
        while (Move move = moveSelector.nextMove()) {
            gameHistory_.addMove(move);
            MoveRevertData moveRevertData =  chessBoard.doMove(move);
            Score score = -quiescenceSearch(-beta, -alpha);
            chessBoard.undoMove(move, moveRevertData);
            gameHistory_.pop();
            if (score > currentScore) {
                currentScore = score;
                if (score > alpha) {
                    alpha = score;
                    if (score >= beta) {
                        break;
                    }
                }
            }
        }


        return currentScore;

    }

    Move Search::alphaBetaRoot(int depth) {
        numLeaves++;
        startingDepth = depth;
        gameHistory_.clear();
        Score alpha = -SCORE_INFINITY;
        Score beta = SCORE_INFINITY;

        numLowerBound = 0;
        numUpperBound = 0;
        numExactBound = 0;

        Move bestMove;
        Move ttBestMove = Move::invalid();

        bool ttEntryFound;
        TransPositionTable::Entry &ttEntry = transPositionTable.probe(chessBoard.getHashKey(), ttEntryFound);
        if (ttEntryFound) {
            ttBestMove = ttEntry.bestMove();
        }

        MoveSelector moveSelector{chessBoard, true, ttBestMove, this};

        while (Move move = moveSelector.nextMove()) {
            gameHistory_.addMove(move);
            MoveRevertData moveRevertData = chessBoard.doMove(move);
            Score score = -alphaBeta(-beta, -alpha, depth - 1);
            gameHistory_.pop();
            chessBoard.undoMove(move, moveRevertData);
            if (score > alpha) {
                alpha = score;
                bestMove = move;
            }

            cout << "Move: " << move << " score: " << score << "\n";
        }

        bestLineScore = alpha;
        return bestMove;
    }

    void Search::onError() const {
        cout << gameHistory_;
        chessBoard.printBitboards();
        exit(987);
    }

    uint64_t Search::perft(int depth) {
        MoveChunk moveChunk{};
        uint64_t numNodes = 0;
        chessBoard.generateMoves<ALL>(moveChunk);
        if (depth == 1) {
            return moveChunk.moveList.size();
        }
        for (auto &move : moveChunk.moveList) {
            MoveRevertData moveRevertData = chessBoard.doMove(move);
            numNodes += perft(depth - 1);
            chessBoard.undoMove(move, moveRevertData);
        }
        return numNodes;
    }
}
