class SimpleMovingPiece extends Piece{
    moveSimple(position, board, moveList, shiftRow, shiftCol){
        const shiftedPosition = position.clone().shift(shiftRow, shiftCol);
        if(shiftedPosition.isWithinBounds()){
            switch (board.get(shiftedPosition).getMoveType(this.colorType)){
                case Piece.MoveType.CAPTURE_KING:
                    //king was captured, illegal position
                    return true;
                case Piece.MoveType.CAPTURE:
                case Piece.MoveType.MOVE:
                    moveList.push(new SimpleMove(position ,shiftedPosition));
            }
        }
        return false;
    }
}

