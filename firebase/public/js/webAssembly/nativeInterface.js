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
        Module._calculateMoves();
    }

    get(piecePosition){
        const piecesById = [Pieces.WHITE_PAWN, Pieces.WHITE_KNIGHT, Pieces.WHITE_KING, Pieces.BLACK_PAWN, Pieces.BLACK_KNIGHT, Pieces.BLACK_KING, Pieces.EMPTY];
        const pieceId = Module._pieceOn(piecePosition.squareId);
        return piecesById[pieceId];
    }

    isValidMoveStart(piecePosition){
        return Module._isLegalMoveStart(piecePosition.squareId);
    }

    allMovesFrom(piecePosition){
        const legalMoves = [];
        const srcSquareId = piecePosition.squareId;
        Module._calculateMovesFrom(srcSquareId);
        PiecePosition.forEveryPosition((dst)=>{
            const isLegalMove = Module._isLegalMoveTo(dst.squareId);
            if(isLegalMove){
                legalMoves.push({dstPos:dst});
            }
        });
        return legalMoves;
    }

    doMoveIfLegal(srcPos, dstPos){
        return Module._doMoveIfLegal(srcPos.squareId, dstPos.squareId);
    }

}