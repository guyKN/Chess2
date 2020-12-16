class ChessSquareHTMLElement extends HTMLDivElement {

    piecePos;

    static flagName = "square";


    static lightBackgroundFlag = "light-background";
    static darkBackgroundFlag = "dark-background";

    static whiteFlag = "white";
    static blackFlag = "black";
    static pawnFlag = "pawn";
    static knightFlag = "knight";
    static bishopFlag = "bishop";
    static rookFlag = "rook";
    static queenFlag = "queen";
    static kingFlag = "king";
    static highLightSrcFlag = "highlight-src";
    static highlightDstFlag = "highlight-dst";

    callback;

    constructor() {
        super();
        this.classList.add();
        this.classList.add(ChessSquareHTMLElement.flagName);
    }

    setPosition(piecePos) {
        this.piecePos = piecePos;
        this.classList.remove(
            ChessSquareHTMLElement.lightBackgroundFlag,
            ChessSquareHTMLElement.darkBackgroundFlag);

        this.classList.add(this.getBackground());
    }

    removePiece() {
        this.classList.remove(ChessSquareHTMLElement.whiteFlag, ChessSquareHTMLElement.blackFlag,
            ChessSquareHTMLElement.pawnFlag, ChessSquareHTMLElement.knightFlag, ChessSquareHTMLElement.bishopFlag,
            ChessSquareHTMLElement.rookFlag, ChessSquareHTMLElement.queenFlag, ChessSquareHTMLElement.kingFlag);
    }

    setPiece(piece) {
        this.removePiece();
        this.classList.add(piece.cssColorFlag, piece.cssTypeFlag);

    }

    setClickCallback(callback){
        this.callback = callback;
        this.addEventListener("click",
            ()=>{
            //console.log(this.piecePos);
            callback.onCellClicked(this.piecePos);
        });
    }

    getBackground() {
        return ((((this.piecePos.row) % 2 + (this.piecePos.col) % 2) === 1) ?
            ChessSquareHTMLElement.darkBackgroundFlag : ChessSquareHTMLElement.lightBackgroundFlag);
    }

    addHighlightSrc(){
        this.classList.add(ChessSquareHTMLElement.highLightSrcFlag);
    }

    addHighlightDst(){
        this.classList.add(ChessSquareHTMLElement.highlightDstFlag);
    }

    removeHighlights(){
        this.classList.remove(ChessSquareHTMLElement.highlightDstFlag, ChessSquareHTMLElement.highLightSrcFlag);
    }


}

customElements.define("chess-square", ChessSquareHTMLElement, {extends: "div"});