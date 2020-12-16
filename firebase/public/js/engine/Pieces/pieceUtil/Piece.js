class Piece{
    colorType;
    pieceType;
    cssColorFlag;
    cssTypeFlag;
    constructor() {
        if (new.target === Piece) {
            throw new TypeError("Cannot construct Abstract instances directly");
        }
    }

    //must have constants: cssColorFlag, cssTypeFlag, ColorType, PieceType


    allLegalMoves(position, board, moveList){
        throw new TypeError("Must override abstract Method");
        //adds moves to moveList, returns true if can capture king, otherwise returns false
    }

    isEmpty(){
        throw new TypeError("Must override abstract Method");
    }

    isKing(){
        throw new TypeError("Must override abstract Method");
    }

    // returns the piece's value. Used for position evaluation.
    // if the piece is white, the value is positive, otherwise the value is negetive
    getValue(position, isEndgame){
        throw new TypeError("Must override abstract Method");
    }

    isSameColorAs(color){
        return this.colorType === color;
    }

    isEnemyOf(color){
        return enemyColor(this.colorType) === color;
    }

    getMoveType(color){
        // if a piece of the specified color where to move into where this piece is, what kind of move would that be.
        if(this.isEmpty()){
            return Piece.MoveType.MOVE;
        }else if(this.isSameColorAs(color)){
            return Piece.MoveType.FRIENDLY_FIRE;
        }else if(this.isKing()){
            return Piece.MoveType.CAPTURE_KING;
        }else {
            return Piece.MoveType.CAPTURE;
        }
    }


    static MoveType = {
        CAPTURE_KING:1,
        CAPTURE:2,
        MOVE:3,
        FRIENDLY_FIRE: 4
    }
}

