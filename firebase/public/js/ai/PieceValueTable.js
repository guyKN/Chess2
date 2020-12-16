class PieceValueTable{
    valuesArray;
    constructor(valuesArray) {
        this.valuesArray = valuesArray;
    }

    clone(){
        return new PieceValueTable(this.valuesArray.clone());
    }

    get(position){
        return this.valuesArray[position.row][position.col];
    }



    flipColor(){
        const newValuesArray = [];
        for (let row = 0; row < BOARD_NUM_ROWS; row++) {
            const slice = [];
            for (let col = 0; col < BOARD_NUM_COLS; col++) {
                slice.push(- this.valuesArray[7-row][col]);
            }
            newValuesArray.push(slice);
        }
        return new PieceValueTable(newValuesArray);
    }

    addBaseValue(baseValue){
        for (let row = 0; row < BOARD_NUM_ROWS; row++) {
            const slice = [];
            for (let col = 0; col < BOARD_NUM_COLS; col++) {
                this.valuesArray[row][col]+=baseValue;
            }
        }
        return this;
    }

    static PAWN_VALUES = new PieceValueTable([
        [0,  0,  0,  0,  0,  0,  0,  0,],
        [50, 50, 50, 50, 50, 50, 50, 50,],
        [10, 10, 20, 30, 30, 20, 10, 10,],
        [5,  5, 10, 25, 25, 10,  5,  5,],
        [0,  0,  0, 20, 20,  0,  0,  0,],
        [5, -5,-10,  0,  0,-10, -5,  5,],
        [5, 10, 10,-20,-20, 10, 10,  5,],
        [0,  0,  0,  0,  0,  0,  0,  0,],
    ]);

    static KNIGHT_VALUES = new PieceValueTable([
        [-50,-40,-30,-30,-30,-30,-40,-50,],
        [-40,-20,  0,  0,  0,  0,-20,-40,],
        [-30,  0, 10, 15, 15, 10,  0,-30,],
        [-30,  5, 15, 20, 20, 15,  5,-30,],
        [-30,  0, 15, 20, 20, 15,  0,-30,],
        [-30,  5, 10, 15, 15, 10,  5,-30,],
        [-40,-20,  0,  5,  5,  0,-20,-40,],
        [-50,-40,-30,-30,-30,-30,-40,-50,],
    ]);

    static BISHOP_VALUES = new PieceValueTable([
        [-20,-10,-10,-10,-10,-10,-10,-20,],
        [-10,  0,  0,  0,  0,  0,  0,-10,],
        [-10,  0,  5, 10, 10,  5,  0,-10,],
        [-10,  5,  5, 10, 10,  5,  5,-10,],
        [-10,  0, 10, 10, 10, 10,  0,-10,],
        [-10, 10, 10, 10, 10, 10, 10,-10,],
        [-10,  5,  0,  0,  0,  0,  5,-10,],
        [-20,-10,-10,-10,-10,-10,-10,-20,],
    ]);

    static ROOK_VALUES = new PieceValueTable([
        [0,  0,  0,  0,  0,  0,  0,  0,],
        [5, 10, 10, 10, 10, 10, 10,  5,],
        [-5,  0,  0,  0,  0,  0,  0, -5,],
        [-5,  0,  0,  0,  0,  0,  0, -5,],
        [-5,  0,  0,  0,  0,  0,  0, -5,],
        [-5,  0,  0,  0,  0,  0,  0, -5,],
        [-5,  0,  0,  0,  0,  0,  0, -5,],
        [0,  0,  0,  5,  5,  0,  0,  0,],
    ]);

    static QUEEN_VALUES = new PieceValueTable([
        [-20,-10,-10, -5, -5,-10,-10,-20,],
        [-10,  0,  0,  0,  0,  0,  0,-10,],
        [-10,  0,  5,  5,  5,  5,  0,-10,],
        [-5,  0,  5,  5,  5,  5,  0, -5,],
        [0,  0,  5,  5,  5,  5,  0, -5,],
        [-10,  5,  5,  5,  5,  5,  0,-10,],
        [-10,  0,  5,  0,  0,  0,  0,-10,],
        [-20,-10,-10, -5, -5,-10,-10,-20],
    ]);

    static KING_VALUES_MIDGAME = new PieceValueTable([
        [-30,-40,-40,-50,-50,-40,-40,-30,],
        [-30,-40,-40,-50,-50,-40,-40,-30,],
        [-30,-40,-40,-50,-50,-40,-40,-30,],
        [-30,-40,-40,-50,-50,-40,-40,-30,],
        [-20,-30,-30,-40,-40,-30,-30,-20,],
        [-10,-20,-20,-20,-20,-20,-20,-10,],
        [20, 20,  0,  0,  0,  0, 20, 20,],
        [20, 30, 10,  0,  0, 10, 30, 20]
    ]);

    static KING_VALUES_ENDGAME = new PieceValueTable([
        [-50,-40,-30,-20,-20,-30,-40,-50,],
        [-30,-20,-10,  0,  0,-10,-20,-30,],
        [-30,-10, 20, 30, 30, 20,-10,-30,],
        [-30,-10, 30, 40, 40, 30,-10,-30,],
        [-30,-10, 30, 40, 40, 30,-10,-30,],
        [-30,-10, 20, 30, 30, 20,-10,-30,],
        [-30,-30,  0,  0,  0,  0,-30,-30,],
        [-50,-30,-30,-30,-30,-30,-30,-50],
    ]);
}