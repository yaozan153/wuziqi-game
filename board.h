#ifndef WUZIQI_BOARD_H
#define WUZIQI_BOARD_H

#include "game.h"

const int CELL_SIZE = 40;
const int BOARD_LEFT = 28;
const int BOARD_TOP = 28;

bool mouseToBoard(int mouseX, int mouseY, int& row, int& col);


bool loadPieceImages(); 

void freePieceImages();

void drawBoard();

void drawPiece(int row, int col, PieceColor piece);

void drawLastMoveMarker(int row, int col); // 在最后落下的棋子中心绘制小红圈。

void drawCrosshair(int row, int col);

#endif
