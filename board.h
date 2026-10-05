#ifndef WUZIQI_BOARD_H
#define WUZIQI_BOARD_H

#include "game.h" // 绘图需要棋盘尺寸和棋子类型。

bool loadPieceImages(); // 开始游戏时加载棋子和木纹图片，成功返回 true。
void freePieceImages(); // 退出前释放图片资源。
void drawBoard();

// 在指定行列的交点上画棋子；仅负责绘图，不修改游戏数据。
void drawPiece(int row, int col, Piece piece);
void drawCrosshair(int row, int col); // 在交点周围画青色四角准星。

#endif
