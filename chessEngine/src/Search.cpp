//
// Created by guykn on 12/30/2020.
//


// todo: speed up by remove unnecassary checks
#include "Search.h"

namespace Chess {

    Score Search::alphaBeta(Score alpha, Score beta, int depthLeft) {
        Key originalKey = chessBoard.getHashKey();
        Indent indent{4 - depthLeft};
        if (depthLeft == 0) {
            numLeaves++;
            Score score = chessBoard.evaluate();
            return score;
        }

        numNonLeafNodes++;

        assert(chessBoard.isOk());

        MoveList moveList;
        GameEndState gameEndState = chessBoard.generateMoves(moveList);
        switch (gameEndState) {
            case DRAW:
                return SCORE_DRAW;
            case MATED:
                return SCORE_MATED;
            case NO_GAME_END:
                break;
        }
        //ChessBoard prevChessboard = chessBoard;
        for (const Move *pMove = moveList.firstMove(); pMove < moveList.lastMove(); pMove++) {
            Move move = *pMove;
            gameHistory_.addMove(move);
            MoveRevertData moveRevertData = chessBoard.doMove(move);
            Score score = -alphaBeta(-beta, -alpha, depthLeft - 1);
            chessBoard.undoMove(move, moveRevertData);
#if FOR_RELEASE == 0
            if (originalKey != chessBoard.getHashKey()) {
                cout << "\ngameHistory:" << gameHistory << "\n";
                cout << "overlapping keys";
                chessBoard.printBitboards();
                assert(false);
            }
#endif
//            if(!chessBoard.samePositionAs(prevChessboard)){
//                cout << "Current Chess board: \n\n";
//                chessBoard.printBitboards();
//                cout << "Previous chess board: \n";
//                prevChessboard.printBitboards();
//                cout << "\n" << gameHistory;
//                assert(false);
//            }
            gameHistory_.pop();
            if (score > alpha) {
                alpha = score;
            }
            if (score >= beta) {
                return beta;
            }
        }
        return alpha;
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
            Move move = *pMove;
            gameHistory_.addMove(move);
            MoveRevertData moveRevertData = chessBoard.doMove(move);
            Score score = -alphaBeta(-beta, -alpha, depth - 1);
            gameHistory_.pop();
            chessBoard.undoMove(move, moveRevertData);
            if (score > alpha) {
                alpha = score;
                bestMove = move;
            }
        }

        bestLineScore = alpha;

        return bestMove;
    }
}