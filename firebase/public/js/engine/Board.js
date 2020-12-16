class Board {

    static NO_PAWN_MOVE_FORWARD_TWO_FLAG = 10;

    static WinState = {
        NO_WINNER: 0,
        WHITE_WINS: 1,
        BLACK_WINS: -1,
        DRAW: 2
    }

    boardArray;
    legalMoves = new MoveList();
    color;
    whiteCanCastleKingSide;
    whiteCanCastleQueenSide;
    blackCanCastleKingSide;
    blackCanCastleQueenSide;
    pawnMoveForward2Column = Board.NO_PAWN_MOVE_FORWARD_TWO_FLAG;

    static instance;

    constructor(boardArray, color, whiteCanCastleKingSide, whiteCanCastleQueenSide, blackCanCastleKingSide, blackCanCastleQueenSide) {
        this.boardArray = boardArray;
        this.color = color;
        this.whiteCanCastleKingSide = whiteCanCastleKingSide;
        this.whiteCanCastleQueenSide = whiteCanCastleQueenSide;
        this.blackCanCastleKingSide = blackCanCastleKingSide;
        this.blackCanCastleQueenSide = blackCanCastleQueenSide;
    }


    get(position) {
        return this.boardArray[position.row][position.col];
    }

    set(position, piece) {
        this.boardArray[position.row][position.col] = piece;
    }

    doMoveIfLegal(srcPos, dstPos) {
        // returns true if the move was done successfully, otherwise returns false
        const move = this.legalMoves.getMoveListFrom(srcPos).getMoveTo(dstPos);
        if (!move) {
            //the move is illegal
            return false;
        }
        move.doMoveOnBoard(this);
        return true;
    }

    placeTempKings(positions) {
        // used to check if the king is in check when castling
        positions.forEach(pos => {
            this.set(pos, getKingFromColor(this.color));
        });
        return this;
    }

    checkIfPositionsAreNotInCheck(positions) {
        return !this.clone().placeTempKings(positions).swapColor().calculateAllLegalMoves();
    }

    calculateAllLegalMoves() {
        //returns true if captures the king
        this.legalMoves = new MoveList();

        //console.log("calculating legal Moves");

        for (let row = 0; row < BOARD_NUM_ROWS; row++) {
            for (let col = 0; col < BOARD_NUM_COLS; col++) {
                const position = new PiecePosition(row, col);
                const piece = this.get(position);
                if (piece.isSameColorAs(this.color)) {
                    if (piece.allLegalMoves(position, this, this.legalMoves.getMoveListFrom(position))) {
                        return true;
                        //if this returns true, that means the king has been captured, and the position is illegal
                    }
                }
            }
        }
        this.checkCastling();
        return false;
    }

    checkCastling() {
        switch (this.color) {
            case ColorsTypes.WHITE:

                if (this.whiteCanCastleQueenSide) {
                    CastleMove.whiteQueenSide.addToMoveListIfLegal(this);
                }
                if(this.whiteCanCastleKingSide){
                    CastleMove.whiteKingSide.addToMoveListIfLegal(this);
                }
                break;
            case ColorsTypes.BLACK:
                if(this.blackCanCastleQueenSide){
                    CastleMove.blackQueenSide.addToMoveListIfLegal(this);
                }

                if(this.blackCanCastleKingSide){
                    CastleMove.blackKingSide.addToMoveListIfLegal(this);
                }
                break;
        }
    }

    verifyLegalMoves() {
        PiecePosition.forEveryPosition(position => {
            const legalMovesFromPos = this.legalMoves.getMoveListFrom(position);
            //console.log("LegalMovesFromPos: ", legalMovesFromPos);
            for (let i = 0; i < legalMovesFromPos.length; i++) {
                const clonedBoardAfterMove = legalMovesFromPos[i].doMoveOnBoard(this.clone());
                //console.log("clonedBoardAfterMove: ", clonedBoardAfterMove);
                //error here
                if (clonedBoardAfterMove.calculateAllLegalMoves()) {
                    //this position is actually illegal!
                    legalMovesFromPos.splice(i, 1); //remove the position
                    i--;
                }
            }
        });
    }

    checkWinner(){
        if(this.legalMoves.isEmpty()){
            if(this.isInCheck()){
                // checkMate, current player loses

                switch (this.color){
                    case ColorsTypes.WHITE:
                        return Board.WinState.BLACK_WINS;
                        // noinspection UnreachableCodeJS
                        break;
                    case ColorsTypes.BLACK:
                        return Board.WinState.WHITE_WINS;
                        // noinspection UnreachableCodeJS
                        break;
                }
            }else {
                return Board.WinState.DRAW;
            }
        }
        return Board.WinState.NO_WINNER;
    }

    evaluate(){
        let currentValue = 0;
        for(let row = 0; row<BOARD_NUM_ROWS; row++) {
            for (let col = 0; col < BOARD_NUM_ROWS; col++) {
                const pos = new PiecePosition(row, col);
                currentValue += this.get(pos).getValue(pos, false);
            }
        }
        return currentValue;
    }

    doBestMove(){
        if(this.color === ColorsTypes.WHITE) {
            getBestMoveWhite(this, 4).doMoveOnBoard(this);
        }else {
            getBestMoveBlack(this, 4).doMoveOnBoard(this);
        }
    }


    isInCheck(){
        return this.clone().swapColor().calculateAllLegalMoves();
    }

    allMovesFrom(position) {
        return this.legalMoves.getMoveListFrom(position);
    }

    clone() {
        return new Board(this.boardArray.clone(), this.color,
            this.whiteCanCastleKingSide, this.whiteCanCastleQueenSide,
            this.blackCanCastleKingSide, this.blackCanCastleQueenSide);
    }

    swapColor() {
        this.color = enemyColor(this.color);
        return this;
    }

    isValidMoveStart(position) {
        return this.get(position).isSameColorAs(this.color);
    }

    static getStartingBoard() {
        const board = new Board(
            [
                [Pieces.BLACK_ROOK, Pieces.BLACK_KNIGHT, Pieces.BLACK_BISHOP, Pieces.BLACK_QUEEN, Pieces.BLACK_KING, Pieces.BLACK_BISHOP, Pieces.BLACK_KNIGHT, Pieces.BLACK_ROOK],
                [Pieces.BLACK_PAWN, Pieces.BLACK_PAWN, Pieces.BLACK_PAWN, Pieces.BLACK_PAWN, Pieces.BLACK_PAWN, Pieces.BLACK_PAWN, Pieces.BLACK_PAWN, Pieces.BLACK_PAWN],
                [Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY],
                [Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY],
                [Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY],
                [Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY, Pieces.EMPTY],
                [Pieces.WHITE_PAWN, Pieces.WHITE_PAWN, Pieces.WHITE_PAWN, Pieces.WHITE_PAWN, Pieces.WHITE_PAWN, Pieces.WHITE_PAWN, Pieces.WHITE_PAWN, Pieces.WHITE_PAWN],
                [Pieces.WHITE_ROOK, Pieces.WHITE_KNIGHT, Pieces.WHITE_BISHOP, Pieces.WHITE_QUEEN, Pieces.WHITE_KING, Pieces.WHITE_BISHOP, Pieces.WHITE_KNIGHT, Pieces.WHITE_ROOK]
            ], ColorsTypes.WHITE, true, true, true, true).clone();
        Board.instance = board;
        return board;
    }
}


Array.prototype.clone = function () {
    const myClone = [];
    for (let i = 0; i < this.length; i++) {
        let arraySlice = [];
        let len2 = this[i].length;
        for (let j = 0; j < len2; j++) {
            arraySlice.push(this[i][j]);
        }
        myClone.push(arraySlice);
    }
    return myClone;
}