//
// Created by guykn on 12/30/2020.
//


// todo: speed up by remove unnecassary checks
#include <TransPositionTable.h>
#include "Search.h"
#include "Uci.h"
#define USE_TRANSPOSITION_TABLE

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

        if(!chessBoard.isOk()){
            cout << "chessBoard not OK\n";
            chessBoard.printBitboards();
            cout << gameHistory_;
            exit(987);
        }
        bool ttEntryFound;
        TransPositionTable::Entry &ttEntry = transPositionTable.probe(chessBoard.getHashKey(), ttEntryFound);
        ttEntry.onVisit(chessBoard.getMoveCount());
        Move ttBestMove = Move::invalid();
        if (ttEntryFound) {
            if (!ttEntry.chessBoard().isOk()){
                cout << "other chessboard not ok\n";
                exit(456);
            }

            if(!chessBoard.samePositionAs(ttEntry.chessBoard())){
                Uci::log("Key collision\n");
                chessBoard.getFen(cout << "current Chessboard: ") << "\n";
                ttEntry.chessBoard().getFen(cout << "ttEntry chessboard: ") << "\n";
                cout << "current key: " << chessBoard.getHashKey();
                exit(321);
            } else {
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
                moveList.addMove(ttBestMove);
                assert(ttBestMove.isOk());
                assert(ttEntry.key() == chessBoard.getHashKey());
            }
        } else{
            ttEntry.setKey(chessBoard.getHashKey());
            ttEntry.setChessBoard(chessBoard);
        }

        ttEntry.startSearching();
        GameEndState gameEndState = chessBoard.generateMoves(moveList);

        switch (gameEndState) {
            case DRAW:
                //todo: handle putting data into transposition table
                ttEntry.stopSearching();
                ttEntry.setBoundType(BOUND_EXACT);
                ttEntry.setDepth(0);
                ttEntry.setScore(SCORE_DRAW);
                return SCORE_DRAW;
            case MATED:
                ttEntry.setBoundType(BOUND_EXACT);
                ttEntry.setScore(SCORE_MATED);
                ttEntry.stopSearching();
                ttEntry.setDepth(0);
                return SCORE_MATED;
            case NO_GAME_END:
                break;
        }

        Score bestScore = -SCORE_INFINITY;
        Move bestMove = Move::invalid();
        for (const Move *pMove = moveList.firstMove(); pMove != moveList.lastMove(); pMove++) {
            if (pMove != moveList.firstMove() && (*pMove == ttBestMove)) {
                //since the best bestMove apears both in the movelist by natural generatin by being pushed to its front, we ignore it
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
                    if (score < beta) {
                        alpha = score;
                    } else{
                        break;
                    }
                }
            }
        }

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

    uint64_t Search::perft(int depth) {
        MoveList moveList;
        uint64_t numNodes = 0;
        chessBoard.generateMoves(moveList);
        if(depth==1){
            return moveList.size();
        }
        for (const Move* move = moveList.firstMove();move!=moveList.lastMove();++move){
            MoveRevertData moveRevertData =  chessBoard.doMove(*move);
            numNodes+=perft(depth-1);
            chessBoard.undoMove(*move, moveRevertData);
        }
        return numNodes;
    }
}
