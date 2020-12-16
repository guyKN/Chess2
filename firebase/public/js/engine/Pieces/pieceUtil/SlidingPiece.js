class SlidingPiece extends Piece {
    //todo: replace with loop instead of recursion

    slidingMove2(srcPos, board, moveList, slideRow, slideCol){
        let dstPos = srcPos;
        while (true){
            dstPos = dstPos.clone().shift(slideRow, slideCol);
            if (!dstPos.isWithinBounds()) {
                return false;
            }
            switch (board.get(dstPos).getMoveType(this.colorType)) {
                case Piece.MoveType.CAPTURE_KING:
                    //king was captured, illegal position
                    return true;
                case Piece.MoveType.CAPTURE:
                    moveList.push(new SimpleMove(srcPos, dstPos));
                    return false;
                case Piece.MoveType.MOVE:
                    moveList.push(new SimpleMove(srcPos, dstPos));
                    break;
                default:
                    return false;
            }
        }
    }

    slidingMove(position, board, moveList, slideRow, slideCol) {
        const shiftedPosition = position.clone().shift(slideRow, slideCol);
        if (shiftedPosition.isWithinBounds()) {
            switch (board.get(shiftedPosition).getMoveType(this.colorType)) {
                case Piece.MoveType.CAPTURE_KING:
                    //king was captured, illegal position
                    return true;
                case Piece.MoveType.CAPTURE:
                    moveList.push(new SimpleMove(shiftedPosition));
                    break;
                case Piece.MoveType.MOVE:
                    moveList.push(new SimpleMove(shiftedPosition));
                    return this.slidingMove(shiftedPosition, board, moveList, slideRow, slideCol);
            }
        }
        return false;
    }

    moveDiagonals(position, board, moveList) {
        return this.slidingMove2(position, board, moveList, 1, 1) ||
            this.slidingMove2(position, board, moveList, -1, 1) ||
            this.slidingMove2(position, board, moveList, 1, -1) ||
            this.slidingMove2(position, board, moveList, -1, -1);
    }

    moveStraight(position, board, moveList) {
        return this.slidingMove2(position, board, moveList, 1, 0) ||
            this.slidingMove2(position, board, moveList, 0, 1) ||
            this.slidingMove2(position, board, moveList, -1, 0) ||
            this.slidingMove2(position, board, moveList, 0, -1);
    }
}

