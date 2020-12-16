class Rook extends SlidingPiece {
    pieceType = PieceTypes.ROOK;
    cssTypeFlag = CssPieceFlags.ROOK;
    baseValue;

    static BASE_VALUE = 500;

    constructor(colorType) {
        super();
        this.colorType = colorType;
        this.cssColorFlag = getCssColorFlag(colorType);
        this.baseValue = flipValueIfBlack(Rook.BASE_VALUE, colorType);

        if(this.colorType === ColorsTypes.WHITE){
            this.valueTable = PieceValueTable.ROOK_VALUES.clone().addBaseValue(Rook.BASE_VALUE);
        }else {
            this.valueTable = PieceValueTable.ROOK_VALUES.clone().addBaseValue(Rook.BASE_VALUE).flipColor();
        }
    }

    isEmpty() {
        return false;
    }

    isKing() {
        return false;
    }

    allLegalMoves(position, board, moveList) {
        return this.moveStraight(position, board, moveList);
    }

    getValue(position, isEndgame) {
        return this.valueTable.get(position);
    }

    static initRooks(){
        Pieces.WHITE_ROOK = new Rook(ColorsTypes.WHITE);
        Pieces.BLACK_ROOK = new Rook(ColorsTypes.BLACK);
    }
}

