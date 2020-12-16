class SimpleMove extends Move {
    srcPos;
    constructor(srcPos, dstPos) {
        super();
        this.dstPos = dstPos;
        this.srcPos = srcPos;
    }

    doMoveOnBoard(board) {
        board.set(this.dstPos, board.get(this.srcPos));
        board.set(this.srcPos, Pieces.EMPTY);
        board.swapColor();
        board.pawnMoveForward2Column = Board.NO_PAWN_MOVE_FORWARD_TWO_FLAG;

        if (this.srcPos.isEqual(CastleMove.blackKingSide.castleData.movement.king.src)) {
            // this is moving the king, so castling will become illegal from both sides
            board.blackCanCastleKingSide = false;
            board.blackCanCastleQueenSide = false;
        } else if (this.srcPos.isEqual(CastleMove.blackKingSide.castleData.movement.rook.src)) {
            //the king-side rook is moving, so black can't castle king-side
            board.blackCanCastleKingSide = false;
        } else if (this.srcPos.isEqual(CastleMove.blackQueenSide.castleData.movement.rook.src)) {
            //the king-side rook is moving, so black can't castle king-side
            board.blackCanCastleQueenSide = false;
        } else if (this.srcPos.isEqual(CastleMove.whiteKingSide.castleData.movement.king.src)) {
            // this is moving the king, so castling will become illegal from both sides
            board.whiteCanCastleQueenSide = false;
            board.whiteCanCastleKingSide = false;
        } else if (this.srcPos.isEqual(CastleMove.whiteKingSide.castleData.movement.rook.src)) {
            //the king-side rook is moving, so white can't castle king-side
            board.whiteCanCastleKingSide = false;
        } else if (this.srcPos.isEqual(CastleMove.whiteQueenSide.castleData.movement.rook.src)) {
            //the king-side rook is moving, so white can't castle king-side
            board.whiteCanCastleQueenSide = false;
        }
        return board;
    }
}