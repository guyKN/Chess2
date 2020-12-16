class MoveList {
    moves = [
        [[], [], [], [], [], [], [], []],
        [[], [], [], [], [], [], [], []],
        [[], [], [], [], [], [], [], []],
        [[], [], [], [], [], [], [], []],
        [[], [], [], [], [], [], [], []],
        [[], [], [], [], [], [], [], []],
        [[], [], [], [], [], [], [], []],
        [[], [], [], [], [], [], [], []],
    ];

    constructor() {

    }

    getMoveListFrom(position) {
        return this.moves[position.row][position.col];
    }

    isEmpty() {
        return this.moves.isEmpty2D();
    }

    forEveryMove(action) {
        for (let row = 0; row < BOARD_NUM_ROWS; row++) {
            for (let col = 0; col < BOARD_NUM_ROWS; col++) {
                const srcPos = new PiecePosition(row, col);
                const moves = this.getMoveListFrom(srcPos);
                for (let i = 0; i < moves.length; i++) {
                    const move = moves[i];
                    action(move);
                }

                }
        }
    }
}

Array.prototype.getMoveTo = function (dstPos) {
    for (let i = 0; i < this.length; i++) {
        if (dstPos.isEqual(this[i].dstPos)) return this[i];
    }
    return null;
}

Array.prototype.isEmpty2D = function () {
    for (let i = 0; i < this.length; i++) {
        for (let j = 0; j < this[0].length; j++) {
            if (this[i][j].length !== 0) {
                return false;
            }
        }
    }
    return true;
}

