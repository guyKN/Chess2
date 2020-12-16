class PawnForward2Move extends Move{
    dstPos;
    srcPos;

    constructor(srcPos,dstPos) {
        super();
        this.dstPos = dstPos;
        this.srcPos = srcPos;
    }

    doMoveOnBoard(board) {
        board.set(this.dstPos, board.get(this.srcPos));
        board.set(this.srcPos, Pieces.EMPTY);
        board.swapColor();
        board.pawnMoveForward2Column = this.srcPos.col;
        return board;
    }
}