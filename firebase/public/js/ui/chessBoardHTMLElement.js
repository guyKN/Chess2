class ChessBoardHTMLElement extends HTMLTableElement {

    boardPosition;
    winnerDisplay;
    lastClickPos = null;
    isGameActive;
    restartButton;
    pieceMovementAudio;

    static instance;
    static AI_MOVE_DELAY = 1;

    constructor() {
        super();
        this.classList.add(ChessBoardHTMLElement.flagName);
        this.winnerDisplay = document.getElementById("winner-display");
        this.restartButton = document.getElementById("restart");

        this.setUpBoard();
        this.isGameActive = true;
        this.pieceMovementAudio = new Audio("assets/pieceMovementSound4.mp3");

       ChessBoardHTMLElement.instance = this;
    }

    static numCols = BOARD_NUM_COLS;
    static numRows = BOARD_NUM_ROWS;
    static flagName = "board";


    setUpBoard() {
        for (let row = 0; row < ChessBoardHTMLElement.numRows; row++) {
            const tr = this.insertRow(-1);
            for (let col = 0; col < ChessBoardHTMLElement.numCols; col++) {
                const td = tr.insertCell(-1);
                const square = document.createElement("div", {is: "chess-square"});
                square.setPosition(new PiecePosition(row, col));
                square.setClickCallback(this);
                td.appendChild(square);
            }
        }
    }

    startGame(){
        this.setBoardPosition(Board.getStartingBoard());
        this.isGameActive = true;
        this.winnerDisplay.innerText = "";
        this.restartButton.style.visibility = "hidden";
    }

    restartGame(){
        this.boardPosition.resetBoard();
        this.boardPosition.calculateAllLegalMoves();
        this.updateBoard();
        this.isGameActive = true;
        this.winnerDisplay.innerText = "";
        this.restartButton.style.visibility = "hidden";
    }

    setPiece(pos, piece) {
        this.getSquare(pos).setPiece(piece);
    }

    getSquare(piecePos) {
        return this.rows[piecePos.row].cells[piecePos.col].firstChild;
    }

    setBoardPosition(boardPosition) {
        this.boardPosition = boardPosition;
        this.updateBoard();
        this.boardPosition.calculateAllLegalMoves();
    }

    updateBoard(){
        PiecePosition.forEveryPosition((position) => {
            this.getSquare(position).setPiece(
                this.boardPosition.get(position)
            );
        });
    }

    undoMove(){
        this.removeAllHighlight();
        this.boardPosition.undoMove();
        this.boardPosition.calculateAllLegalMoves();
        this.updateBoard();

    }

    removeAllHighlight(){
        PiecePosition.forEveryPosition((position)=>{
            this.getSquare(position).removeHighlights();
        });
    }

    highlightDst(moveArray){
        //console.log("positions: ", moveArray);
        moveArray.forEach((move)=>{
            this.getSquare(move.dstPos).addHighlightDst();
        });
    }

    onCellClicked(position) {
        if(!this.isGameActive) return;
        if (this.lastClickPos) {
            //console.log("a pos was clicked");
            if(this.boardPosition.doMoveIfLegal(this.lastClickPos, position)) {
                // the move was legal
                this.pieceMovementAudio.play();
                this.boardPosition.calculateAllLegalMoves();
                this.updateBoard();
                this.removeAllHighlight();
                this.lastClickPos = null;
                this.showThreats();
                if(!this.checkWinner()){
                    setTimeout( ()=>{this.doAiMove()});
                }
                return;
            }else{
                this.removeAllHighlight();
                this.lastClickPos = null;
            }
        }

        if(this.boardPosition.isValidMoveStart(position)) {
            //console.log("isValidMoveStart");
            this.lastClickPos = position;
            this.getSquare(position).addHighlightSrc();

            const allMoves = this.boardPosition.allMovesFrom(position);
            this.highlightDst(allMoves);
        }
        this.showThreats();
    }

    checkWinner() {
        const winState = this.boardPosition.checkWinner();
        //console.log("WinState: ", winState);
        switch (winState) {
            case Board.WinState.WHITE_WINS:
                this.whiteWins();
                return true;
            case Board.WinState.BLACK_WINS:
                this.blackWins();
                return true;
            case Board.WinState.DRAW:
                this.draw();
                return true;
            default:
                return false;
        }
    }

    showThreats(){
        PiecePosition.forEveryPosition((position)=>{
            if(this.boardPosition.isThreatTo(position)){
                this.getSquare(position).addHighlightDst();
            }
        });
    }

    doAiMove(){
        this.boardPosition.doAiMove();
        this.boardPosition.calculateAllLegalMoves();
        this.updateBoard();
        this.pieceMovementAudio.play();
        if(!this.checkWinner()){
            this.isGameActive = true;
        }
    }

    whiteWins(){
        this.winnerDisplay.innerText = "White Wins!";
        this.isGameActive = false;
        this.restartButton.style.visibility = "visible";
    }

    blackWins(){
        this.winnerDisplay.innerText = "Black Wins!";
        this.isGameActive = false;
        this.restartButton.style.visibility = "visible";
    }

    draw(){
        this.winnerDisplay.innerText = "It's a Draw!";
        this.isGameActive = false;
        this.restartButton.style.visibility = "visible";
    }
}

customElements.define('chess-board', ChessBoardHTMLElement, {extends: 'table'});