class EmptyPiece extends Piece{
    constructor() {
        super();
    }

    colorType = ColorsTypes.EMPTY;
    pieceType = PieceTypes.EMPTY;

    cssColorFlag = CssColorFlags.EMPTY;
    cssTypeFlag = CssPieceFlags.EMPTY;

    isLegalMove(move, board) {
        return false;
    }
    allLegalMoves(position, board, moveList) {
        return false;
    }

    isEmpty() {
        return true;
    }

    getValue(position, isEndgame) {
        return 0;
    }
}

Pieces.EMPTY = new EmptyPiece();