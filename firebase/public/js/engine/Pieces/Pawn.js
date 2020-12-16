class Pawn extends Piece {
    promotionRow;
    valueTable;

    static BASE_VALUE = 100;


    constructor(color, jumpRow, direction, promotionRow, enPassantRow) {
        // color = WHITE or BLACK
        // jumpRow: 7 or 2
        // Direction 1 or -1
        super();
        this.colorType = color;
        this.jumpRow = jumpRow;
        this.direction = direction;
        this.promotionRow = promotionRow;
        this.enPassantRow = enPassantRow;


        if(this.colorType === ColorsTypes.WHITE){
            this.cssColorFlag = CssColorFlags.WHITE;
            this.valueTable = PieceValueTable.PAWN_VALUES.clone().addBaseValue(Pawn.BASE_VALUE);
        }else {
            this.cssColorFlag = CssColorFlags.BLACK;
            this.valueTable = PieceValueTable.PAWN_VALUES.clone().addBaseValue(Pawn.BASE_VALUE).flipColor();
        }
    }

    pieceType = PieceTypes.PAWN;
    cssTypeFlag = CssPieceFlags.PAWN;


    isEmpty() {
        return false;
    }
    isKing() {
        return false;
    }

    getValue(position, isEndgame) {
        return this.valueTable.get(position);
    }

    allLegalMoves(position, board, moveList) {
        //adds elements to the moveList array, and returns true if this is an illegal Position
        return this.moveForwardV2(position, board, moveList) ||
            (this.captureDiagonal(position, board, moveList, -1)) ||
            (this.captureDiagonal(position, board, moveList, 1)) ||
            (this.doEnPassant(position, board, moveList));
    }

    doEnPassant(srcPos, board, moveList){
        if(srcPos.row !== this.enPassantRow) return false;
        const deltaCol = board.pawnMoveForward2Column - srcPos.col;
        if(deltaCol === 1 || deltaCol === -1){
            const dstPos = new PiecePosition(this.enPassantRow + this.direction ,board.pawnMoveForward2Column);
            moveList.push(new EnPassantMove(srcPos, dstPos, this.direction));
        }
    }

    captureDiagonal(position, board, moveList, horizontalDirection) {
        let shiftedPosition = position.clone().shift(this.direction, horizontalDirection);
            ////console.log("shiftedDiagonalPosition: ");
        //     //console.log(shiftedPosition);
        if (shiftedPosition.isWithinBounds()) {
            ////console.log("within bounds");
            const moveType = board.get(shiftedPosition)
                .getMoveType(this.colorType);

            switch (moveType) {
                case Piece.MoveType.CAPTURE_KING:
                    //king has been captured. alert that this position is illegal.
                    return true;
                case Piece.MoveType.CAPTURE:
                    this.doMove(moveList, position, shiftedPosition);
            }
        }
        return false;
    }

    moveForwardV2(srcPos, board, moveList){
        let dstPos = srcPos.clone().shiftVertical(this.direction);
        switch (board.get(dstPos).getMoveType(this.colorType)){
            case Piece.MoveType.MOVE:
                this.doMove(moveList, srcPos, dstPos);
                if(srcPos.row === this.jumpRow) {
                    dstPos = srcPos.clone().shiftVertical(this.direction * 2);
                    switch (board.get(dstPos).getMoveType(this.colorType)) {
                        case Piece.MoveType.MOVE:
                            moveList.push(new PawnForward2Move(srcPos, dstPos));
                            break;
                    }
                }
            break;
        }
    }

    moveForwardIfPossible(srcPos, board, moveList, mayMoveTwoSquares, isMovingForward2) {
        const shiftedPosition = srcPos.clone().shiftVertical(this.direction);
        if(shiftedPosition.isWithinBounds()) {
            ////console.log("within bounds!");
            const moveType = board.get(shiftedPosition).getMoveType(this.colorType);
            switch (moveType) {
                case Piece.MoveType.MOVE:
                    ////console.log("MoveType.MOVE!");
                    this.doMove(moveList, shiftedPosition, isMovingForward2)
                    if (mayMoveTwoSquares) {
                        ////console.log("recalling this function.");
                        this.moveForwardIfPossible(shiftedPosition ,board, moveList, false, true);
                    }
            }
        }
        return false;
    }

    doMove(moveList, srcPos, dstPos){
        if(dstPos.row === this.promotionRow){
            moveList.push(new PromotionMove(srcPos, dstPos, getQueenFromColor(this.colorType)));
        }else {
            moveList.push(new SimpleMove(srcPos, dstPos));
        }
    }

    static initPawns(){
        Pieces.WHITE_PAWN = new Pawn(ColorsTypes.WHITE, 6, -1, 0, 3);
        Pieces.BLACK_PAWN = new Pawn(ColorsTypes.BLACK, 1, 1, 7,4);
    }
}