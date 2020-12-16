class CastleMove extends Move{

    dstPos;
    castleData;

    constructor(castleData) {
        super();
        this.castleData = castleData;
        this.dstPos = castleData.movement.king.dst;
    }

    doMoveOnBoard(board) {
        // move king
        board.set(this.castleData.movement.king.dst, board.get(this.castleData.movement.king.src));
        board.set(this.castleData.movement.king.src, Pieces.EMPTY);

        // move rook
        board.set(this.castleData.movement.rook.dst, board.get(this.castleData.movement.rook.src));
        board.set(this.castleData.movement.rook.src, Pieces.EMPTY);
        board.pawnMoveForward2Column = Board.NO_PAWN_MOVE_FORWARD_TWO_FLAG;
        board.swapColor();

        //now that the player has castled, they can't any more, so disable Castling
        switch (this.castleData.color){
            case ColorsTypes.WHITE:
                board.whiteCanCastleKingSide = false;
                board.whiteCanCastleQueenSide = false;
                break;
            case ColorsTypes.BLACK:
                board.blackCanCastleKingSide = false;
                board.blackCanCastleQueenSide = false;
                break;
        }
        return board;
    }

    addToMoveListIfLegal(board){
        if(this.isLegalCastle(board)){
            board.legalMoves.getMoveListFrom(this.castleData.movement.king.src).push(this);
        }
    }

    isLegalCastle(board){
        return this.checkEmptySquares(board) && board.checkIfPositionsAreNotInCheck(this.castleData.mustNotBeInCheck);
    }

    checkEmptySquares(board){
        for(let i=0; i<this.castleData.mustBeEmpty.length;i++){
            if(!board.get(this.castleData.mustBeEmpty[i]).isEmpty()){
                return false;
            }
        }
        return true;
    }

    static blackQueenSide = new CastleMove({
        color: ColorsTypes.BLACK,
        movement:{
            king: {
                src:new PiecePosition(0,4),
                dst: new PiecePosition(0,2)
            },
            rook:{
                src: new PiecePosition(0,0),
                dst: new PiecePosition(0,3)
            }
        },

        mustBeEmpty:[
            new PiecePosition(0,1),
            new PiecePosition(0,2),
            new PiecePosition(0,3)],

        mustNotBeInCheck:[
            new PiecePosition(0,2),
            new PiecePosition(0,3),
            new PiecePosition(0,4)]
    });

    static blackKingSide = new CastleMove({
        color: ColorsTypes.BLACK,
        movement:{
            king:{
                src:new PiecePosition(0,4),
                dst: new PiecePosition(0,6)
            },
            rook:{
                src:new PiecePosition(0,7),
                dst: new PiecePosition(0,5)
            }
        },
        mustBeEmpty:[
            new PiecePosition(0,5),
            new PiecePosition(0,6)],

        mustNotBeInCheck:[
            new PiecePosition(0,4),
            new PiecePosition(0,5),
            new PiecePosition(0,6)]
    });

    static whiteQueenSide = new CastleMove({
        color: ColorsTypes.WHITE,
        movement:{
            rook:{
                src:new PiecePosition(7,0),
                dst: new PiecePosition(7,3)
            },
            king:{
                src:new PiecePosition(7,4),
                dst: new PiecePosition(7,2)
            }
        },
        mustBeEmpty:[
            new PiecePosition(7,1),
            new PiecePosition(7,2),
            new PiecePosition(7,3)],

        mustNotBeInCheck:[
            new PiecePosition(7,2),
            new PiecePosition(7,3),
            new PiecePosition(7,4)]
    });

    static whiteKingSide =new CastleMove({
        color: ColorsTypes.WHITE,

        movement:{
            rook:{
                src:new PiecePosition(7,7),
                dst: new PiecePosition(7,5)
            },
            king:{
                src:new PiecePosition(7,4),
                dst: new PiecePosition(7,6)
            }
        },
        mustBeEmpty:[
            new PiecePosition(7,5),
            new PiecePosition(7,6)],


        mustNotBeInCheck:[
            new PiecePosition(7,4),
            new PiecePosition(7,5),
            new PiecePosition(7,6)]
    });
}
