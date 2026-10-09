// 文件职责：人机模式：选择玩家颜色、限制玩家回合、在电脑回合调用 AI 落子。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#include "human_ai.h" // 引入 "human_ai.h"，提供本文件使用的类型和函数声明。
#include "../../board.h"
#include "ai.h" // 引入 "ai.h"，提供本文件使用的类型和函数声明。
#include "../../ui/menu.h" // 引入 "../../ui/menu.h"，提供本文件使用的类型和函数声明。
#include "../../ui/match.h" // 引入 "../../ui/match.h"，提供本文件使用的类型和函数声明。
#include <ege.h> // 引入 <ege.h>，提供本文件使用的类型和函数声明。
using namespace ege; // 允许直接使用该命名空间中的名称，省略 ege:: 或 std:: 前缀。

void drawHumanAiChoice(int mouseX, int mouseY) // 函数入口：绘制玩家选黑、选白及返回菜单按钮。
{ // 开始上方函数、条件、循环或类型的作用域。
    setbkmode(TRANSPARENT); // 设置文字背景透明方式；参数为 `TRANSPARENT)`。
    setcolor(EGERGB(45, 37, 26)); // 设置后续线条和文字颜色；参数为 `EGERGB(45, 37, 26))`。
    setfont(36, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `36, 0, L"微软雅黑")`。
    const wchar_t* title = L"请选择你的棋子"; // 声明 title：标题文字或处理后的存档名称；按右侧表达式初始化。
    outtextxy(400 - textwidth(title) / 2, 165, title); // 在指定坐标绘制文字；参数为 `400 - textwidth(title) / 2, 165, title)`。
    setfont(20, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `20, 0, L"微软雅黑")`。
    const wchar_t* note = L"黑棋先手，白棋后手"; // 声明 note：本行使用的局部数据；按右侧表达式初始化。
    outtextxy(400 - textwidth(note) / 2, 225, note); // 在指定坐标绘制文字；参数为 `400 - textwidth(note) / 2, 225, note)`。
    drawButton(280, 280, 240, 48, L"执黑 · 我先下", mouseX, mouseY); // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
    drawButton(280, 346, 240, 48, L"执白 · 电脑先下", mouseX, mouseY); // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
    drawButton(280, 412, 240, 48, L"返回菜单", mouseX, mouseY); // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
}

PieceColor humanAiChoice(int x, int y) // 函数入口：返回选色按钮对应的棋色，未命中返回 EMPTY。
{ // 开始上方函数、条件、循环或类型的作用域。
    if (insideButton(x, y, 280, 280, 240, 48)) return BLACK_PIECE; // 检查 `if (insideButton(x, y, 280, 280, 240, 48)) return BLACK_PIECE`；条件成立时执行括号之后或下一行的处理。
    if (insideButton(x, y, 280, 346, 240, 48)) return WHITE_PIECE; // 检查 `if (insideButton(x, y, 280, 346, 240, 48)) return WHITE_PIECE`；条件成立时执行括号之后或下一行的处理。
    return EMPTY; // 返回 `EMPTY`，将结果交给调用者。
}

bool humanAiChoiceBack(int x, int y) // 函数入口：检查选色页面的返回按钮位置。
{ // 开始上方函数、条件、循环或类型的作用域。
    return insideButton(x, y, 280, 412, 240, 48); // 返回 `insideButton(x, y, 280, 412, 240, 48)`，将结果交给调用者。
}

void drawHumanAiScreen(const GameState& game, PieceColor humanPiece, int mouseX, int mouseY) // 函数入口：按玩家颜色与当前回合绘制人机状态，满盘无赢家显示和棋。
{ // 开始上方函数、条件、循环或类型的作用域。
    const wchar_t* colors = humanPiece == BLACK_PIECE // 声明 colors：玩家和电脑执棋颜色说明；按右侧表达式初始化。
                           ? L"你执黑，电脑执白" : L"你执白，电脑执黑"; // 条件表达式：`? L"你执黑，电脑执白" : L"你执白，电脑执黑"`；根据条件选择两个值之一。
    const bool full = game.moves.size() >= BOARD_SIZE * BOARD_SIZE; // 声明 full：棋盘是否已经下满；按右侧表达式初始化。
    const bool humanTurn = game.currentPlayer == humanPiece; // 声明 humanTurn：当前是否轮到玩家；按右侧表达式初始化。
    const wchar_t* status = game.winner != EMPTY ? matchStatus(game) // 声明 status：当前状态说明；按右侧表达式初始化。
                            : full ? L"棋盘已满，和棋" // 构造函数初始化列表：设置网络库、套接字及状态的初始值。
                            : humanTurn ? L"轮到你落子" : L"轮到电脑落子"; // 构造函数初始化列表：设置网络库、套接字及状态的初始值。
    drawMatch(game, L"人机对决", colors, status, // 绘制共用棋盘页；canMove 只控制准星，实际合法性由落子函数检查。
              humanTurn && !full, mouseX, mouseY); // 承接上方表达式的参数、条件或初值：`humanTurn && !full, mouseX, mouseY)`。
}

bool humanAiMove(GameState& game, PieceColor humanPiece, int x, int y) // 函数入口：仅允许玩家回合处理点击并通过共用规则落子。
{ // 开始上方函数、条件、循环或类型的作用域。
    if ((humanPiece != BLACK_PIECE && humanPiece != WHITE_PIECE) || // 检查 `if ((humanPiece != BLACK_PIECE && humanPiece != WHITE_PIECE) ||`；条件成立时执行括号之后或下一行的处理。
        game.currentPlayer != humanPiece) // 承接上方表达式的参数、条件或初值：`game.currentPlayer != humanPiece)`。
        return false; // 返回 false：本次条件不满足或操作未成功；具体含义见当前函数说明。
    int row, col; // 声明 row：棋盘行号；参数或长度由本行给出。
    return mouseToBoard(x, y, row, col) && placePiece(game, row, col); // 先检查鼠标是否在棋盘点击范围；成功才落子，利用 && 的短路避免使用无效行列。
}

bool computerAiMove(GameState& game, PieceColor humanPiece) // 函数入口：电脑回合选最佳空位并落子；结束、满盘或玩家回合不动作。
{ // 开始上方函数、条件、循环或类型的作用域。
    if ((humanPiece != BLACK_PIECE && humanPiece != WHITE_PIECE) || // 检查 `if ((humanPiece != BLACK_PIECE && humanPiece != WHITE_PIECE) ||`；条件成立时执行括号之后或下一行的处理。
        game.currentPlayer == humanPiece || game.winner != EMPTY || // 继续上方条件：`game.currentPlayer == humanPiece || game.winner != EMPTY ||`；&& 要求同时满足，|| 表示任一成立。
        game.moves.size() >= BOARD_SIZE * BOARD_SIZE) // 承接上方表达式的参数、条件或初值：`game.moves.size() >= BOARD_SIZE * BOARD_SIZE)`。
        return false; // 返回 false：本次条件不满足或操作未成功；具体含义见当前函数说明。
    const PieceColor aiPiece = humanPiece == BLACK_PIECE ? WHITE_PIECE : BLACK_PIECE; // 声明 aiPiece：电脑棋色；按右侧表达式初始化。
    int row, col; // 声明 row：棋盘行号；参数或长度由本行给出。
    return findBestAiMove(game, aiPiece, row, col) && placePiece(game, row, col); // 返回 `findBestAiMove(game, aiPiece, row, col) && placePiece(game, row, col)`，将结果交给调用者。
}
