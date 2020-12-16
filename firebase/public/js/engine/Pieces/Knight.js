class Knight extends SimpleMovingPiece {
    colorType;
    cssColorFlag;

    static BASE_VALUE = 320;

    constructor(colorType) {
        super();
        this.colorType = colorType;
        this.cssColorFlag = getCssColorFlag(colorType);

        if(this.colorType === ColorsTypes.WHITE){
            this.valueTable = PieceValueTable.KNIGHT_VALUES.clone().addBaseValue(Knight.BASE_VALUE);
        }else {
            this.valueTable = PieceValueTable.KNIGHT_VALUES.clone().addBaseValue(Knight.BASE_VALUE).flipColor();
        }
    }

    pieceType = PieceTypes.KNIGHT;
    cssTypeFlag = CssPieceFlags.KNIGHT;

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
        return this.moveSimple(position, board, moveList,2, 1) ||
            this.moveSimple(position, board, moveList,-2, 1) ||
            this.moveSimple(position, board, moveList,2, -1) ||
            this.moveSimple(position, board, moveList,-2, -1) ||
            this.moveSimple(position, board, moveList,1, 2) ||
            this.moveSimple(position, board, moveList,-1, 2) ||
            this.moveSimple(position, board, moveList,1, -2) ||
            this.moveSimple(position, board, moveList,-1, -2);
    }
    static initKnights(){
        Pieces.WHITE_KNIGHT = new Knight(ColorsTypes.WHITE);
        Pieces.BLACK_KNIGHT = new Knight(ColorsTypes.BLACK);
    }
}
