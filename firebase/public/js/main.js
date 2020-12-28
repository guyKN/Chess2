initPieces();

const container = document.getElementById("board-container");
const chessBoard = document.createElement("table", {is: "chess-board"});
container.appendChild(chessBoard);



function restartGame(){
    chessBoard.restartGame();
}

function testSpeed(iterations){
    const start = Date.now();
    for (let i=0;i<iterations;i++){
        Board.instance.calculateAllLegalMoves();
    }
    const end = Date.now();
    const dt = end - start;
    console.log("total time: " + dt);
    console.log("Moves per second:", (iterations)/(dt/1000));
    console.log(end-start);
}