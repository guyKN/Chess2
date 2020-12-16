const STARTING_ALPHA_BETA = 1e7;
const CHECKMATE_SCORE = 1e6;
const DRAW_SCORE = 0;

function alphaBetaWhite(board, alpha, beta, depthLeft) {
    if (depthLeft === 0) {
        return board.evaluate();
    }
    let numLegalMoves = 0;
    if(board.calculateAllLegalMoves()){
        //this position allows a king capture
        return CHECKMATE_SCORE;
    }
    for (let row = 0; row < BOARD_NUM_ROWS; row++) {
        for (let col = 0; col < BOARD_NUM_ROWS; col++) {
            const srcPos = new PiecePosition(row, col);
            const moves = board.legalMoves.getMoveListFrom(srcPos);
            for (let i = 0; i < moves.length; i++) {
                numLegalMoves++;
                const move = moves[i];
                const moveScore = alphaBetaBlack(move.doMoveOnBoard(board.clone()), alpha, beta, depthLeft - 1);
                if (moveScore >= beta) {
                    return beta;
                }
                if(moveScore>alpha){
                    alpha = moveScore;
                }
            }
        }
    }
    if(numLegalMoves === 0){
        if(board.isInCheck()){
            return -CHECKMATE_SCORE;
        }else {
            return -DRAW_SCORE;
        }
    }
    return alpha;
}

function alphaBetaBlack(board, alpha, beta, depthLeft){
    if (depthLeft === 0) {
        return board.evaluate();
    }
    let numLegalMoves = 0;
    if(board.calculateAllLegalMoves()){
        //this position allows a king capture
        return -CHECKMATE_SCORE;
    }


    for (let row = 0; row < BOARD_NUM_ROWS; row++) {
        for (let col = 0; col < BOARD_NUM_ROWS; col++) {
            const srcPos = new PiecePosition(row, col);
            const moves = board.legalMoves.getMoveListFrom(srcPos);
            for (let i = 0; i < moves.length; i++) {
                numLegalMoves++;
                const move = moves[i];
                const moveScore = alphaBetaWhite(move.doMoveOnBoard(board.clone()), alpha, beta, depthLeft - 1);
                if (moveScore <= alpha) {
                    return alpha;
                }
                if(moveScore<beta){
                    beta = moveScore;
                }
            }
        }
    }

    if(numLegalMoves === 0){
        if(board.isInCheck()){
            return CHECKMATE_SCORE;
        }else {
            return DRAW_SCORE;
        }
    }
    return beta;
}

function getBestMoveWhite(board, depth) {
    let numLegalMoves = 0;
    let bestMove = null;
    let alpha = -STARTING_ALPHA_BETA;
    let beta = STARTING_ALPHA_BETA;
    board.calculateAllLegalMoves();
    for (let row = 0; row < BOARD_NUM_ROWS; row++) {
        for (let col = 0; col < BOARD_NUM_ROWS; col++) {
            const srcPos = new PiecePosition(row, col);
            const moves = board.legalMoves.getMoveListFrom(srcPos);
            for (let i = 0; i < moves.length; i++) {
                numLegalMoves++;
                const move = moves[i];
                const moveScore = alphaBetaBlack(move.doMoveOnBoard(board.clone()), alpha, beta, depth - 1);

                if(moveScore>alpha){
                    alpha = moveScore;
                    bestMove = move;
                }
            }
        }
    }
    return bestMove;
}

function getBestMoveBlack(board, depth) {
    let numLegalMoves = 0;
    let bestMove = null;
    let alpha = -STARTING_ALPHA_BETA;
    let beta = STARTING_ALPHA_BETA;
    board.calculateAllLegalMoves();
    for (let row = 0; row < BOARD_NUM_ROWS; row++) {
        for (let col = 0; col < BOARD_NUM_ROWS; col++) {
            const srcPos = new PiecePosition(row, col);
            const moves = board.legalMoves.getMoveListFrom(srcPos);
            for (let i = 0; i < moves.length; i++) {
                numLegalMoves++;
                const move = moves[i];
                const moveScore = alphaBetaWhite(move.doMoveOnBoard(board.clone()), alpha, beta, depth - 1);

                if(moveScore<beta){
                    beta = moveScore;
                    bestMove = move;
                }
            }
        }
    }
    return bestMove;
}