//
// Created by guykn on 12/30/2020.
//


// todo: speed up by remove unnecassary checks
#include <TransPositionTable.h>
#include "Search.h"

namespace Chess {

    Score Search::alphaBeta(Score alpha, Score beta, int depthLeft) {
        if (depthLeft == 0) {
            numLeaves++;
            Score score = chessBoard.evaluate();
            return score;
        }
        Key originalKey = chessBoard.getHashKey();

        cout << "searching node\n";

        numNonLeafNodes++;

        assert(chessBoard.isOk());

        bool ttEntryFound;
        TransPositionTable::Entry &ttEntry = transPositionTable.probe(chessBoard.getHashKey(), ttEntryFound);
        ttEntry.onVisit(chessBoard.getMoveCount());
        Move ttBestMove = Move::invalid();
        MoveList moveList;
        if (ttEntryFound) {
            if (ttEntry.isCurrentlySearched()) {
                // repeated position, but to save time, we just a call it a draw instead of waiting for threethold repetition
                return SCORE_DRAW;
            }
            if (ttEntry.depth() >= depthLeft) {
                if (ttEntry.isExact() ||
                    (ttEntry.isUpperBound() && ttEntry.score() <= alpha) ||
                    (ttEntry.isLowerBound() && ttEntry.score() >= beta)) {
                    return ttEntry.score();
                }
            }
            ttBestMove = ttEntry.bestMove();
            assert(ttBestMove.isOk());
            moveList.addMove(ttBestMove);
        } else{
            ttEntry.setKey(chessBoard.getHashKey());
        }

        ttEntry.startSearching();

        GameEndState gameEndState = chessBoard.generateMoves(moveList);
        assert(ttBestMove == Move::invalid() || moveList.contains(ttBestMove));
        switch (gameEndState) {
            case DRAW:
                //todo: handle putting data into transposition table
                return SCORE_DRAW;
            case MATED:
                return SCORE_MATED;
            case NO_GAME_END:
                break;
        }

        bool ttBestMoveInList = false;

        Score bestScore = -SCORE_INFINITY;
        Move bestMove = Move::invalid();
        //todo: replace less than with unequal to
        for (const Move *pMove = moveList.firstMove(); pMove < moveList.lastMove(); pMove++) {
            if (pMove != moveList.firstMove() && *pMove == ttBestMove) {
                //since the best bestMove apears both in the movelist by natural generatin by being pushed to its front, we ignore it
                ttBestMoveInList = true;
                continue;
            }
            gameHistory_.addMove(*pMove);
            MoveRevertData moveRevertData = chessBoard.doMove(*pMove);
            Score score = -alphaBeta(-beta, -alpha, depthLeft - 1);
            chessBoard.undoMove(*pMove, moveRevertData);
#if FOR_RELEASE == 0
            if (originalKey != chessBoard.getHashKey()) {
                cout << "\ngameHistory:" << gameHistory << "\n";
                cout << "overlapping keys";
                chessBoard.printBitboards();
                assert(false);
            }
#endif
            gameHistory_.pop();
            //todo: changed order, make sure this isn't completely wrong
            if (score > bestScore) {
                bestScore = score;
                bestMove = *pMove;
                if (score > alpha) {
                    if (score < beta) {
                        alpha = score;
                    } else{
                        break;
                    }
                }
            }
        }

        if(ttEntryFound){
            cout << "found ttEntry and got to end\n";
        }

        assert(ttBestMoveInList || !ttEntryFound);



        ttEntry.stopSearching();
        ttEntry.setBestMove(bestMove);
        if (bestScore<alpha){
            ttEntry.setBoundType(BOUND_UPPER);
        } else if (bestScore>beta){
            ttEntry.setBoundType(BOUND_LOWER);
        } else{
            ttEntry.setBoundType(BOUND_EXACT);
        }
        ttEntry.setScore(bestScore);
        ttEntry.setDepth(depthLeft);

        assert(ttEntry.key() == chessBoard.getHashKey()); //the entry can't be overwritten

        return bestScore;
    }

    Move Search::alphaBetaRoot(int depth) {
        gameHistory_.clear();
        Score alpha = -SCORE_INFINITY;
        Score beta = SCORE_INFINITY;

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

        return bestMove;
    }
}