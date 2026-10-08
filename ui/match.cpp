#include "match.h" // 引入 "match.h"，提供本文件使用的类型和函数声明。
#include "menu.h" // 引入 "menu.h"，提供本文件使用的类型和函数声明。
#include "../board.h" // 引入 "../board.h"，提供本文件使用的类型和函数声明。
#include <ege.h> // 引入 <ege.h>，提供本文件使用的类型和函数声明。
using namespace ege; // 允许直接使用该命名空间中的名称，省略 ege:: 或 std:: 前缀。

const wchar_t* matchStatus(const GameState& game) // 函数入口：根据赢家和满盘状态返回胜负、和棋或轮次文字。
{ // 开始上方函数、条件、循环或类型的作用域。
    if (game.winner == EMPTY && game.moves.size() >= BOARD_SIZE * BOARD_SIZE) // 检查 `if (game.winner == EMPTY && game.moves.size() >= BOARD_SIZE * BOARD_SIZE)`；条件成立时执行括号之后或下一行的处理。
        return L"棋盘已满，和棋"; // 返回 `L"棋盘已满，和棋"`，将结果交给调用者。
    return game.winner == BLACK_PIECE ? L"黑棋获胜" // 返回 `game.winner == BLACK_PIECE ? L"黑棋获胜"`，将结果交给调用者。
           : game.winner == WHITE_PIECE ? L"白棋获胜" // 构造函数初始化列表：设置网络库、套接字及状态的初始值。
           : game.currentPlayer == BLACK_PIECE ? L"轮到黑棋" : L"轮到白棋"; // 构造函数初始化列表：设置网络库、套接字及状态的初始值。
}

void drawMatch(const GameState& game, const wchar_t* title, const wchar_t* description, // 函数入口：绘制共用棋盘页；canMove 只控制准星，实际合法性由落子函数检查。
               const wchar_t* status, bool canMove, int mouseX, int mouseY) // 声明 status：当前状态说明；参数或长度由本行给出。
{ // 开始上方函数、条件、循环或类型的作用域。
    drawBoard(); // 先画木纹，再绘制十三条横竖网格和五个星位。
    for (const Move& move : game.moves) // 循环推进：`const Move& move : game.moves)`；逐项处理棋格、方向、字符或列表元素。
        drawPiece(move.row, move.col, move.piece); // 根据棋色选图片，将透明棋子居中画到交点。
    int row, col; // 声明 row：棋盘行号；参数或长度由本行给出。
    if (canMove && game.winner == EMPTY && mouseToBoard(mouseX, mouseY, row, col) // 检查 `if (canMove && game.winner == EMPTY && mouseToBoard(mouseX, mouseY, row, col)`；条件成立时执行括号之后或下一行的处理。
        && game.board[row][col] == EMPTY) // 继续上方条件：`&& game.board[row][col] == EMPTY)`；&& 要求同时满足，|| 表示任一成立。
        drawCrosshair(row, col); // 在空交点周围画四角准星，结束后恢复线宽。

    setfont(28, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `28, 0, L"微软雅黑")`。
    setbkmode(TRANSPARENT); // 设置文字背景透明方式；参数为 `TRANSPARENT)`。
    setcolor(EGERGB(45, 37, 26)); // 设置后续线条和文字颜色；参数为 `EGERGB(45, 37, 26))`。
    outtextxy(565, 90, title); // 在指定坐标绘制文字；参数为 `565, 90, title)`。
    setfont(22, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `22, 0, L"微软雅黑")`。
    outtextxy(565, 145, status); // 在指定坐标绘制文字；参数为 `565, 145, status)`。
    setfont(18, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `18, 0, L"微软雅黑")`。
    outtextxy(565, 200, description); // 在指定坐标绘制文字；参数为 `565, 200, description)`。
    outtextxy(565, 235, L"Esc 返回菜单"); // 在指定坐标绘制文字；参数为 `565, 235, L"Esc 返回菜单")`。
    drawButton(560, 430, 210, 48, L"返回菜单", mouseX, mouseY); // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
}

bool matchBack(int x, int y) // 函数入口：检查共用棋盘页的返回菜单按钮。
{ // 开始上方函数、条件、循环或类型的作用域。
    return insideButton(x, y, 560, 430, 210, 48); // 返回 `insideButton(x, y, 560, 430, 210, 48)`，将结果交给调用者。
}
