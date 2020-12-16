//
// Created by guykn on 12/14/2020.
//

#ifndef CHESS_JSINTERFACE_H
#define CHESS_JSINTERFACE_H

#include <climits>
#include "types.h"
#include "ChessBoard.h"
#include "iostream"
#include "Bitboards.h"

using namespace Chess;
using std::cout;

extern "C" {
void initData();
int pieceOn(int square);
void calculateMoves();
bool isLegalMoveStart(int square);
void calculateMovesFrom(int square);
bool isLegalMoveTo(int dstSquare);
bool doMoveIfLegal(int srcSquare, int dstSquare);
bool currentPlayer();
void printMoves();
void resetBoard();
bool is64bit();
};

ChessBoard chessBoard = ChessBoard();
MoveList moveList;

Square selectedSquare;
MoveList movesFromSquare;

void initData(){
    initLookupTables();
}

int pieceOn(int square) {
    return chessBoard.getPieceOn(static_cast<Square>(square));
}

void calculateMoves() {
    moveList = MoveList();
    chessBoard.generateMoves(moveList);
}

bool isLegalMoveStart(int square) {
    return chessBoard.isLegalMoveStart(static_cast<Square>(square));
}

void calculateMovesFrom(int square) {
    movesFromSquare = MoveList();
    selectedSquare = static_cast<Square>(square);
    moveList.movesFrom(selectedSquare, movesFromSquare);
}

bool isLegalMoveTo(int dstSquare) {
    return movesFromSquare.getMoveFromInputData(MoveInputData(selectedSquare, static_cast<Square>(dstSquare))).isOk();
}

bool doMoveIfLegal(int srcSquare, int dstSquare) {
    Move move = movesFromSquare.getMoveFromInputData(
            MoveInputData(static_cast<Square>(srcSquare), static_cast<Square>(dstSquare)));
    if(!move.isOk()){
        return false;
    } else{
        chessBoard.doMove(move);
        return true;
    }
}

void printMoves(){
    cout << moveList;
}

bool currentPlayer(){
    return chessBoard.getCurrentPlayer();
}

void resetBoard(){
    chessBoard = ChessBoard();
}

#endif //CHESS_JSINTERFACE_H
