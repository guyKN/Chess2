//
// Created by guykn on 12/30/2020.
//


// todo: speed up by remove unnecassary checks
#include "Search.h"

namespace Chess {

    Score Search::alphaBeta(Score alpha, Score beta, int depthLeft) {
        Indent indent{4 - depthLeft};
        if(PRINT_DEBUG) if constexpr(PRINT_DEBUG) cout << indent
             << "Calling alphaBeta() with depthLeft = " << depthLeft << " alpha = " << alpha << " beta = " << beta
             << "\n";
        if (depthLeft == 0) {
            numLeaves++;
            Score score = chessBoard.evaluate();
            if constexpr(PRINT_DEBUG) cout << indent << "Depth is zero. Evaluation score: " << score << "\n";
            return score;
        }

        numNonLeafNodes++;

        chessBoard.assertOk(); //todo: remove, or replace with assert isOk()

        MoveList moveList;
        GameEndState gameEndState = chessBoard.generateMoves(moveList);
        if constexpr(PRINT_DEBUG) cout << indent << "Generating Moves. Number of moves generated: " << moveList.size() << "\n";
        switch (gameEndState) {
            case DRAW:
                if constexpr(PRINT_DEBUG) cout << indent << "Found a draw returning SCORE_DRAW\n";
                return SCORE_DRAW;
            case MATED:
                if constexpr(PRINT_DEBUG) cout << indent << "Mated. Returning SCORE_MATED\n";
                return SCORE_MATED;
            case NO_GAME_END:
                break;
        }
        if constexpr(PRINT_DEBUG) cout << indent <<  "Iterating through moves. \n";
        //ChessBoard prevChessboard = chessBoard;
        for (const Move *pMove = moveList.firstMove(); pMove < moveList.lastMove(); pMove++) {
            Move move = *pMove;
            if constexpr(PRINT_DEBUG) cout << indent << "Found move: " << move << "\n";
            gameHistory_.addMove(move);
            MoveRevertData moveRevertData = chessBoard.doMove(move);
            Score score = -alphaBeta(-beta, -alpha, depthLeft - 1);
            chessBoard.undoMove(move, moveRevertData);
/*
            if(!chessBoard.samePositionAs(prevChessboard)){
                cout << "Current Chess board: \n\n";
                chessBoard.printBitboards();
                cout << "Previous chess board: \n";
                prevChessboard.printBitboards();
                cout << "\n" << gameHistory;
                assert(false);
            }
*/
            gameHistory_.pop();
            if constexpr(PRINT_DEBUG) cout << indent
                 << "Returning to previous call with depthLeft = " << depthLeft << " alpha = " << alpha << " beta = " << beta
                 << "\n";
            if (score > alpha) {
                if constexpr(PRINT_DEBUG) cout << indent << "Score is greater than alpha. Increasing alpha to score.\n";
                alpha = score;
            }
            if (score >= beta) {
                if constexpr(PRINT_DEBUG) cout << indent << "Score is greater than beta. There exist better moves. \n";
                return beta;
            }
        }
        if constexpr(PRINT_DEBUG) cout << indent << "Looked at all possible moves. Alpha for best move is: " << alpha << "\n";
        return alpha;
    }

    Move Search::alphaBetaRoot(int depth) {
        gameHistory_.clear();
        if constexpr(PRINT_DEBUG) cout << "Doing alpha beta search from root\n";
        Score alpha = -SCORE_INFINITY;
        Score beta = SCORE_INFINITY;

        Move bestMove;

        MoveList moveList;
        GameEndState gameEndState = chessBoard.generateMoves(moveList);

        if constexpr(PRINT_DEBUG) cout << "Generating moves. Found " << moveList.size() << " moves\n";


        assert(gameEndState == NO_GAME_END && depth > 0);


        for (const Move *pMove = moveList.firstMove(); pMove < moveList.lastMove(); pMove++) {
            Move move = *pMove;
            gameHistory_.addMove(move);
            if constexpr(PRINT_DEBUG) cout << "Trying move: " << move << "\n";
            MoveRevertData moveRevertData = chessBoard.doMove(move);
            Score score = -alphaBeta(-beta, -alpha, depth - 1);
            gameHistory_.pop();
            chessBoard.undoMove(move, moveRevertData);
            if constexpr(PRINT_DEBUG) cout << "Score for move: " << score << "\n";
            if (score > alpha) {
                if constexpr(PRINT_DEBUG) cout << "Score is greater than alpha increasing alpha\n";
                alpha = score;
                bestMove = move;
            }
        }


        if constexpr(PRINT_DEBUG) cout << "searched through all moves. best move is: " << bestMove << "\n";

        return bestMove;
    }
}