let moduleReady = false;
Module.onRuntimeInitialized = function () {
    moduleReady = true;
    Module._initData();
    ChessBoardHTMLElement.instance.setBoardPosition(new NativeChessBoard());
}

class NativeChessBoard{

    static instance;

    constructor() {

    }

    calculateAllLegalMoves(){
        try {
            Module._calculateMoves();
        }catch (e) {
            alert("Error in C++");
            throw e;
        }
    }

    get(piecePosition){
        try {
            const piecesById = [Pieces.WHITE_PAWN, Pieces.WHITE_KNIGHT, Pieces.WHITE_BISHOP, Pieces.WHITE_ROOK, Pieces.WHITE_QUEEN, Pieces.WHITE_KING, Pieces.BLACK_PAWN, Pieces.BLACK_KNIGHT, Pieces.BLACK_BISHOP, Pieces.BLACK_ROOK, Pieces.BLACK_QUEEN, Pieces.BLACK_KING, Pieces.EMPTY];
            const pieceId = Module._pieceOn(piecePosition.squareId);
            return piecesById[pieceId];
        }catch (e){
            alert("Error in C++");
            throw e;
        }
    }

    isValidMoveStart(piecePosition){
        try {
            return Module._isLegalMoveStart(piecePosition.squareId);
        }catch (e){
            alert("Error in C++");
            throw e;
        }
    }

    allMovesFrom(piecePosition){
        try {
            const legalMoves = [];
            const srcSquareId = piecePosition.squareId;
            Module._calculateMovesFrom(srcSquareId);
            PiecePosition.forEveryPosition((dst) => {
                const isLegalMove = Module._isLegalMoveTo(dst.squareId);
                if (isLegalMove) {
                    legalMoves.push({dstPos: dst});
                }
            });
            return legalMoves;
        }catch (e){
            alert("Error in C++");
            throw e;
        }
    }

    doMoveIfLegal(srcPos, dstPos){
        try {
            return Module._doMoveIfLegal(srcPos.squareId, dstPos.squareId);
        }catch (e){
            alert("Error in C++");
            throw e;
        }
    }

    checkWinner(){
        return Module._checkWinner();
    }

    isThreatTo(position){
        return false;
    }

    resetBoard(){
        Module._resetBoard();
    }

}