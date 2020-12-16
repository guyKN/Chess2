const BOARD_NUM_ROWS = 8;
const BOARD_NUM_COLS = 8;

const ColorsTypes = {
    WHITE: 1,
    EMPTY: 0,
    BLACK: -1
}

const PieceTypes = {
    EMPTY: 0,
    PAWN: 1,
    KNIGHT: 2,
    BISHOP: 3,
    ROOK: 4,
    QUEEN: 5,
    KING: 6
}

const CssColorFlags = {
    EMPTY: "empty",
    BLACK: "black",
    WHITE: "white"
}

const CssPieceFlags = {
    PAWN: "pawn",
    KNIGHT: "knight",
    BISHOP: "bishop",
    ROOK: "rook",
    QUEEN: "queen",
    KING: "king",
    EMPTY: "empty"
};

function enemyColor(colorType){
    return -colorType;
}

function getCssColorFlag(colorType){
    switch (colorType){
        case ColorsTypes.WHITE:
            return CssColorFlags.WHITE;
        case ColorsTypes.BLACK:
            return CssColorFlags.BLACK;
        case ColorsTypes.EMPTY:
            return CssColorFlags.EMPTY;
    }
}


function getKingFromColor(color){
    switch (color){
        case ColorsTypes.WHITE:
            return Pieces.WHITE_KING;
        case ColorsTypes.BLACK:
            return Pieces.BLACK_KING;
        default:
            throw new RangeError("must be white Piece or black piece");
    }
}

function getQueenFromColor(color){
    switch (color){
        case ColorsTypes.WHITE:
            return Pieces.WHITE_QUEEN;
        case ColorsTypes.BLACK:
            return Pieces.BLACK_QUEEN;
        default:
            throw new RangeError("must be white Piece or black piece");
    }
}

function flipValueIfBlack(value, colorType){
    return value * colorType;
}

const Pieces = {};

function initPieces(){
    Pawn.initPawns();
    Bishop.initBishops();
    Knight.initKnights();
    Rook.initRooks();
    Queen.initQueens();
    King.initKings();
}