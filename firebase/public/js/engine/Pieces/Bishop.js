class Bishop extends SlidingPiece {

    static BASE_VALUE = 330;

    pieceType = PieceTypes.BISHOP;
    cssTypeFlag = CssPieceFlags.BISHOP;


    constructor(colorType) {
        super();
        this.colorType = colorType;
        this.cssColorFlag = getCssColorFlag(colorType);

        if(this.colorType === ColorsTypes.WHITE){
            this.valueTable = PieceValueTable.BISHOP_VALUES.clone().addBaseValue(Bishop.BASE_VALUE);
        }else {
            this.valueTable = PieceValueTable.BISHOP_VALUES.clone().addBaseValue(Bishop.BASE_VALUE).flipColor();
        }

    }

    isEmpty() {
        return false;
    }

    isKing() {
        return false;
    }

    allLegalMoves(position, board, moveList) {
        return this.moveDiagonals(position, board, moveList);
    }

    getValue(position, isEndgame) {
        return this.valueTable.get(position);
    }

    static initBishops(){
        Pieces.WHITE_BISHOP = new Bishop(ColorsTypes.WHITE);
        Pieces.BLACK_BISHOP = new Bishop(ColorsTypes.BLACK);
    }
}

