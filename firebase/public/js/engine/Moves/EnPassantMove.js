class EnPassantMove extends Move {
    dstPos;
    direction;
    srcPos;

    constructor(srcPos ,dstPos, direction) {
        super();
        this.dstPos = dstPos;
        this.direction = direction;
        this.srcPos = srcPos;
    }

    doMoveOnBoard(board) {
        board.set(this.dstPos, board.get(this.srcPos));
        board.set(this.srcPos, Pieces.EMPTY);
        const capturedPawnPosition = this.dstPos.clone().shiftVertical(-this.direction);
        ////console.log("capturedPawnPosition: ", capturedPawnPosition);
        board.set(capturedPawnPosition, Pieces.EMPTY);

        board.swapColor();
        board.pawnMoveForward2Column = Board.NO_PAWN_MOVE_FORWARD_TWO_FLAG;

        return board;
    }
}