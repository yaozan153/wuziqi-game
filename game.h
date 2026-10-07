#ifndef WUZIQI_GAME_H 
#define WUZIQI_GAME_H

#include <vector> 

const int BOARD_SIZE = 13; 

enum PieceColor { EMPTY = 0, BLACK_PIECE = 1, WHITE_PIECE = 2 };

struct Move 
{ 
    int row; 
    int col;
    PieceColor piece; 
};

struct GameState 
{ 
    PieceColor board[BOARD_SIZE][BOARD_SIZE] = {};
    PieceColor currentPlayer = BLACK_PIECE; 
    PieceColor winner = EMPTY; 
    std::vector<Move> moves; 
};





bool placePiece(GameState& game, int row, int col); 


bool undoLastMove(GameState& game); 


bool checkWin(const GameState& game, int row, int col); 
#endif 
