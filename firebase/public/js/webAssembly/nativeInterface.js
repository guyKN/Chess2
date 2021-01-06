let moduleReady = false;
Module.onRuntimeInitialized = function () {
    moduleReady = true;
    Module._WASM_initData();
    ChessBoardHTMLElement.instance.setBoardPosition(new NativeChessBoard());
}

class NativeChessBoard{

    static instance;

    constructor() {
        NativeChessBoard.instance = this;
    }

    calculateAllLegalMoves(){
        try {
            Module._WASM_calculateMoves();
        }catch (e) {
            alert("Error in C++");
            throw e;
        }
    }

    get(piecePosition){
        try {
            const piecesById = [Pieces.WHITE_PAWN, Pieces.WHITE_KNIGHT, Pieces.WHITE_BISHOP, Pieces.WHITE_ROOK, Pieces.WHITE_QUEEN, Pieces.WHITE_KING, Pieces.BLACK_PAWN, Pieces.BLACK_KNIGHT, Pieces.BLACK_BISHOP, Pieces.BLACK_ROOK, Pieces.BLACK_QUEEN, Pieces.BLACK_KING, Pieces.EMPTY];
            const pieceId = Module._WASM_pieceOn(piecePosition.squareId);
            return piecesById[pieceId];
        }catch (e){
            alert("Error in C++");
            throw e;
        }
    }

    isValidMoveStart(piecePosition){
        try {
            return Module._WASM_isLegalMoveStart(piecePosition.squareId);
        }catch (e){
            alert("Error in C++");
            throw e;
        }
    }

    allMovesFrom(piecePosition){
        try {
            const legalMoves = [];
            const srcSquareId = piecePosition.squareId;
            Module._WASM_calculateMovesFrom(srcSquareId);
            PiecePosition.forEveryPosition((dst) => {
                const isLegalMove = Module._WASM_isLegalMoveTo(dst.squareId);
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
            return Module._WASM_doMoveIfLegal(srcPos.squareId, dstPos.squareId);
        }catch (e){
            alert("Error in C++");
            throw e;
        }
    }

    checkWinner(){
        return Module._WASM_checkWinner();
    }

    isThreatTo(position){
        return false;
    }

    resetBoard(){
        Module._WASM_resetBoard();
    }

    printDebug(){
        Module._WASM_printBitboards();
    }

    undoMove(){
        Module._WASM_undoMove();
    }

    doAiMove(){
        try{
        Module._WASM_doAiMove(5);
        }catch (e){
            alert("Error in C++");
            throw e;
        }
    }

    gotoPos(){
        try{
        Module._WASM_gotoPos();
        }catch (e){
            alert("Error in C++");
            throw e;
        }
    }
}
