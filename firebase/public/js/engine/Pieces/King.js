class King extends SimpleMovingPiece {
    colorType;
    cssColorFlag;
    baseValue;


    static WHITE_VALUE = 0;

    constructor(colorType) {
        super();
        this.colorType = colorType;
        this.cssColorFlag = getCssColorFlag(colorType);
        this.baseValue = flipValueIfBlack(King.WHITE_VALUE, colorType);

        if(this.colorType === ColorsTypes.WHITE){
            this.valueTable = PieceValueTable.KING_VALUES_MIDGAME.clone();
        }else {
            this.valueTable = PieceValueTable.KING_VALUES_MIDGAME.clone().flipColor();
        }

    }

    getValue(position, isEndgame) {
        return this.valueTable.get(position);
    }

    pieceType = PieceTypes.KING;
    cssTypeFlag = CssPieceFlags.KING;

    isEmpty() {
        return false;
    }

    isKing() {
        return true;
    }

    allLegalMoves(position, board, moveList) {
        return this.moveSimple(position, board, moveList, 1, 1) ||
            this.moveSimple(position, board, moveList,1, 0) ||
            this.moveSimple(position, board, moveList,1, -1) ||
            this.moveSimple(position, board, moveList,0, 1) ||
            this.moveSimple(position, board, moveList,0, -1) ||
            this.moveSimple(position, board, moveList,-1, 1) ||
            this.moveSimple(position, board, moveList,-1, 0) ||
            this.moveSimple(position, board, moveList,-1, -1);
    }

    static initKings(){
        Pieces.WHITE_KING = new King(ColorsTypes.WHITE);
        Pieces.BLACK_KING = new King(ColorsTypes.BLACK);
    }
}