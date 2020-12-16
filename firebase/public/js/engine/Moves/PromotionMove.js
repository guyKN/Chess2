class PromotionMove extends Move{
    newPiece;
    srcPos;
    constructor(srcPos,dstPos, newPiece) {
        super();
        this.dstPos = dstPos;
        this.newPiece = newPiece;
        this.srcPos = srcPos;
    }

    doMoveOnBoard(board) {
        board.set(this.dstPos, this.newPiece);
        board.set(this.srcPos, Pieces.EMPTY);
        board.swapColor();
        board.pawnMoveForward2Column = Board.NO_PAWN_MOVE_FORWARD_TWO_FLAG;
        return board;
    }
}