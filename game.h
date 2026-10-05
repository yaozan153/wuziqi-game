#ifndef WUZIQI_GAME_H
#define WUZIQI_GAME_H

#include <vector>

const int BOARD_SIZE = 13;
const int CELL_SIZE = 40; 
const int BOARD_LEFT = 28; 
const int BOARD_TOP = 28; 

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
    Piece winner = EMPTY; // 没有赢家时为 EMPTY，获胜后保存棋子颜色。
    std::vector<Move> moves; // 棋步按先后顺序存储，初始为空；最后一个元素是最新一步。
};


// 准星和落子共用坐标换算，成功时输出从 0 开始的行列编号。
bool mouseToBoard(int mouseX, int mouseY, int& row, int& col);
// 根据行列尝试落子，只更新游戏数据；失败时返回 false。
// & 表示引用：函数修改的就是调用者的 game，不是它的副本。
bool placePiece(GameState& game, int row, int col);

bool checkWin(const GameState& game, int row, int col);
#endif
