class PiecePosition {
    row;
    col;

    constructor(row, col) {
        this.row = row;
        this.col = col;
    }

    static fromSquareId(squareId) {
        return new PiecePosition(7-(squareId / 8), squareId % 8);
    }

    get squareId() {
        return (7-this.row) * 8 + this.col;
    }

    clone() {
        return new PiecePosition(this.row, this.col);
    }

    isWithinBounds() {
        return this.row >= 0 &&
            this.row < BOARD_NUM_ROWS &&
            this.col >= 0 &&
            this.col < BOARD_NUM_COLS;
    }

    shift(rowShift, colShift) {
        this.row += rowShift;
        this.col += colShift;
        return this;
    }

    shiftVertical(shift) {
        this.row += shift;
        return this;
    }

    shiftHorizontal(shift) {
        this.col += shift;
        return this;
    }

    shiftDiagonalLeft(shift) {
        this.row += shift;
        this.col += shift;
        return this;
    }

    shiftDiagonalRight(shift) {
        this.row += shift;
        this.col -= shift;
        return this;
    }

    isEqual(position) {
        return this.row === position.row &&
            this.col === position.col;
    }

    static forEveryPosition(action) {
        for (let row = 0; row < BOARD_NUM_ROWS; row++) {
            for (let col = 0; col < BOARD_NUM_COLS; col++) {
                action(new PiecePosition(row, col));
            }
        }
    }
}
