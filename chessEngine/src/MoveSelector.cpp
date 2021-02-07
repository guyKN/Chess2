//
// Created by guykn on 1/20/2021.
//

#include <MoveSelector.h>
#include <Move.h>

namespace Chess {
    void MoveSelector::initWinningCaptures() {
        stage = WINNING_CAPTURES;
        chessBoard.threatData = &threatData;
        chessBoard.calculateAllThreats();
        if (!chessBoard.checksOk()) {
            cout << "Current player is giving check!" << std::endl;
            if(search != nullptr) {
                search->onError();
            }
        }
        chessBoard.generateMoves<CAPTURES>(moveChunk);
        // sort captures by static exchange evaluation
        std::sort(moveChunk.moveList.begin(), moveChunk.moveList.end(), std::greater<>());
        currentMove = moveChunk.moveList.begin();
    }

    void MoveSelector::initNormalMoves() {
        assert(doNonCaptures);
        chessBoard.threatData = &threatData;
        stage = NORMAL_MOVES;
        moveChunk.moveList.clear();
        chessBoard.generateMoves<NON_CAPTURES>(moveChunk);
        currentMove = moveChunk.moveList.begin();
    }

    void MoveSelector::initLosingCaptures() {
        currentMove = moveChunk.losingCaptures.begin();
        stage = LOSING_CAPTURES;
    }

    Move MoveSelector::nextMove() {
        // todo: speed up by not having to check the switch multiple times.
        // also see if there's a better way to indicate the end of the loop
        while (true) {
            assert(stageOk(stage));
            switch (stage) {
                case TT_MOVE:
                    stage = INIT;
                    if (ttMove && chessBoard.isPartiallyLegal(ttMove)) {
                        return ttMove;
                    }
                    break;
                case INIT:
                    initWinningCaptures();
                    break;
                case WINNING_CAPTURES:
                    if (currentMove == moveChunk.moveList.end()) {
                        if (doNonCaptures){
                            initNormalMoves();
                        } else{
                            initLosingCaptures();
                        }
                        break;
                    }
                    if (!currentMove->isSameAs(ttMove)) {
                        return *(currentMove++);
                    } else {
                        currentMove++;
                    }
                    break;
                case NORMAL_MOVES:
                    assert(doNonCaptures);
                    if (currentMove == moveChunk.moveList.end()) {
                        initLosingCaptures();
                        break;
                    }
                    if (!currentMove->isSameAs(ttMove)) {
                        return *(currentMove++);
                    } else {
                        currentMove++;
                    }
                    break;
                case LOSING_CAPTURES:
                    if (currentMove == moveChunk.losingCaptures.end()) {
                        // end of all the moves.
                        return Move::invalid();
                    }
                    if (!currentMove->isSameAs(ttMove)) {
                        return *(currentMove++);
                    } else {
                        currentMove++;
                    }
                    break;
            }
        }
    }

    std::ostream &operator<<(ostream &os, Stage stage) {
        const static std::string stageToString[NUM_STAGES] = {
                "TT_MOVE",
                "INIT",
                "WINNING_CAPTURES",
                "NORMAL_MOVES",
                "LOSING_CAPTURES"
        };
        return os << stageToString[stage];
    }
}