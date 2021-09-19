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

    // todo: PV search: try to see if https://www.chessprogramming.org/NegaScout improves performance
    // todo: mate distance pruning
    // todo: better use of transposition table
    // todo: null move pruning
    // todo: depth reduction/extensions
    //todo: fuilty pruning. have a threshold for max improvement per depth, and if the score physicly can't surpass that, given the number of moves, return the evaluation

    template<bool pvNode>
    Score Search::alphaBeta(Score alpha, Score beta, int depthLeft) {
        if constexpr (!pvNode) {
            if(beta != alpha+1){
                cout << "beta != alpha+1\n";
                onError();
            }
        } else{
            numPvNodes++;
        }
        if (depthLeft == 0) {
            return quiescenceSearch(alpha, beta);
//            numLeaves++;
//            return chessBoard.evaluate();
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
            if (ttBestMove == Move::invalid()) {
                // the position is checkmate or stalemate. return the score from it.
                return fromTranspositionTable(ttEntry.score(), depthLeft);
            }
            if (!ttBestMove.isOk()) {
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
        bool isFirstMove = true;
        int newDepth = depthLeft - 1;
        while (Move move = moveSelector.nextMove()) {
            gameHistory_.addMove(move);
            MoveRevertData moveRevertData = chessBoard.doMove(move);
            Score score;
            if (!pvNode || !isFirstMove) {
                score = -alphaBeta<false>(-(alpha + 1), -alpha, newDepth);
//                score = -alphaBeta<false>(-alpha, -alpha+1, newDepth);
            }
            if (pvNode && (isFirstMove || (score>alpha && score<beta))) {
                score = -alphaBeta<true>(-beta, -alpha, newDepth);
            }

            chessBoard.undoMove(move, moveRevertData);
            gameHistory_.pop();

            isFirstMove = false;

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
        if (isFirstMove) {
            // no moves were found. This position is either checkmate or stalemate.
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
        if (!bestMove.isOk()) {
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
            MoveRevertData moveRevertData = chessBoard.doMove(move);
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

    constexpr bool PRINT_MOVES = false;

    Move Search::alphaBetaRoot(int depth) {
        numNonLeafNodes++;
        startingDepth = depth;
        gameHistory_.clear();
        Score alpha = -SCORE_INFINITY;
        Score beta = SCORE_INFINITY;

        Move bestMove;
        Move ttBestMove = Move::invalid();

        bool ttEntryFound;
        TransPositionTable::Entry &ttEntry = transPositionTable.probe(chessBoard.getHashKey(), ttEntryFound);
        if (ttEntryFound) {
            ttBestMove = ttEntry.bestMove();
        }
        ttEntry.onVisit(chessBoard.getMoveCount());
        ttEntry.startSearching();
        ttEntry.setKey(chessBoard.getHashKey());

        MoveSelector moveSelector{chessBoard, true, ttBestMove, this};

        bool isFirstMove = true;

        while (Move move = moveSelector.nextMove()) {
            Score score;

            gameHistory_.addMove(move);
            MoveRevertData moveRevertData = chessBoard.doMove(move);

            if(!isFirstMove){
//                score = -alphaBeta<false>(-alpha, -alpha+1, depth-1);
                score = -alphaBeta<false>(-(alpha+1), -alpha, depth-1);
            }
            if(isFirstMove || score>alpha){
                score = -alphaBeta<true>(-beta, -alpha, depth - 1);
            }
            gameHistory_.pop();
            chessBoard.undoMove(move, moveRevertData);
            if (score > alpha) {
                alpha = score;
                bestMove = move;
            }

            isFirstMove = false;

            if constexpr (PRINT_MOVES) cout << "Move: " << move << " score: " << score << "\n";
        }
        //todo: save best move to TT in root
        ttEntry.stopSearching();
        ttEntry.setBoundType(BOUND_EXACT);//todo: actually find the right bound type
        ttEntry.setBestMove(bestMove);
        ttEntry.setScore(alpha);
        ttEntry.setDepth(depth);
        bestLineScore = alpha;
        return bestMove;
    }

    void Search::onError() const {
        cout << "Error!\n";
        cout << gameHistory_;
        chessBoard.printBitboards();
        exit(987);
    }

    uint64_t Search::perft(int depth) {
        if(depth ==0){
            return 1;
        }
        MoveChunk moveChunk{};
        uint64_t numNodes = 0;
        chessBoard.generateThreatsAndMoves(moveChunk);
        for (auto &move : moveChunk.moveList) {
            MoveRevertData moveRevertData = chessBoard.doMove(move);
            numNodes += perft(depth - 1);
            chessBoard.undoMove(move, moveRevertData);
        }
        return numNodes;
    }
}