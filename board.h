#ifndef WUZIQI_BOARD_H
#define WUZIQI_BOARD_H

#include <vector> 
using namespace std;
const int BOARD_SIZE = 13;
const int CELL_SIZE = 40; 
const int BOARD_LEFT = 28; 
const int BOARD_TOP = 28; 

bool loadPieceImages(); // 开始游戏时加载棋子和木纹图片，成功返回 true。
void freePieceImages(); // 退出前释放图片资源。

void drawBoard(); 

enum Piece { EMPTY = 0, BLACK_PIECE = 1, WHITE_PIECE = 2 };

struct Move 
{
    int row; 
    int col; 
    Piece piece; 
};

struct GameState // 把一局游戏的数据集中保存。
{
    Piece board[BOARD_SIZE][BOARD_SIZE] = {}; 
    Piece currentPlayer = BLACK_PIECE; 
    vector<Move> moves; // 棋步按先后顺序存储，初始为空；最后一个元素是最新一步。
};

// 在指定行列的交点上画棋子；仅负责绘图，不修改游戏数据。
void drawPiece(int row, int col, Piece piece);
// 根据鼠标坐标尝试落子；成功返回 true，越界或位置被占用时返回 false。
// & 表示引用：函数修改的就是调用者的 game，不是它的副本。
bool placePiece(GameState& game, int mouseX, int mouseY);

#endif
