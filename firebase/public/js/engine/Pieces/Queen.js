class Queen extends SlidingPiece {
    pieceType = PieceTypes.QUEEN;
    cssTypeFlag = CssPieceFlags.QUEEN;

    static BASE_VALUE = 900;

    constructor(colorType) {
        super();
        this.colorType = colorType;
        this.cssColorFlag = getCssColorFlag(colorType);

        if(this.colorType === ColorsTypes.WHITE){
            this.valueTable = PieceValueTable.QUEEN_VALUES.clone().addBaseValue(Queen.BASE_VALUE);
        }else {
            this.valueTable = PieceValueTable.QUEEN_VALUES.clone().addBaseValue(Queen.BASE_VALUE).flipColor();
        }
    }

    isEmpty() {
        return false;
    }

    isKing() {
        return false;
    }

    allLegalMoves(position, board, moveList) {
        return this.moveDiagonals(position, board, moveList) ||
            this.moveStraight(position, board, moveList);
    }

    getValue(position, isEndgame) {
        return this.valueTable.get(position);
    }

    static initQueens(){
        Pieces.WHITE_QUEEN = new Queen(ColorsTypes.WHITE);
        Pieces.BLACK_QUEEN = new Queen(ColorsTypes.BLACK);
    }
}
