// 文件职责：本地双人模式：点击落子、保存/悔棋/重赛按钮和本次运行内的胜负计数。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#include "local.h" // 引入 "local.h"，提供本文件使用的类型和函数声明。
#include "../../board.h"
#include "../../ui/match.h" // 引入 "../../ui/match.h"，提供本文件使用的类型和函数声明。
#include "../../ui/menu.h" // 引入 "../../ui/menu.h"，提供本文件使用的类型和函数声明。
#include <ege.h> // 引入 <ege.h>，提供本文件使用的类型和函数声明。
#include <string> // 引入 <string>，提供本文件使用的类型和函数声明。

bool localFinished(const GameState& game) // 函数入口：判断本地对局是否获胜或满盘。
{ // 开始上方函数、条件、循环或类型的作用域。
    return game.winner != EMPTY || game.moves.size() >= BOARD_SIZE * BOARD_SIZE; // 已有赢家或棋步达到 13×13 格时，对局结束；满盘但无赢家就是和棋。
}

void updateLocalScore(const GameState& game, LocalScore& score) // 函数入口：先撤销旧结算再登记新结果，重复调用不重复加分，悔棋能撤销结果。
{ // 开始上方函数、条件、循环或类型的作用域。
    const int result = game.winner != EMPTY ? static_cast<int>(game.winner) // 先看有没有赢家：黑胜编码为1、白胜为2；没有赢家时接着下一行判断满盘。
                     : localFinished(game) ? 3 : 0; // 没有赢家但已满盘记为3（和棋），否则记为0（还没结束）。
    if (result == score.countedResult) return; // 本局结果与上次结算相同就不处理，避免每次点击或每帧重复累计。
    if (score.countedResult == 1) --score.blackWins; // 如果本局以前计过黑胜，先减回去；获胜后悔棋时需要撤销这一分。
    if (score.countedResult == 2) --score.whiteWins; // 如果本局以前计过白胜，先撤销这一次白胜统计。
    if (score.countedResult == 3) --score.draws; // 如果本局以前计过和棋，先撤销这一次和棋统计。
    if (result == 1) ++score.blackWins; // 按当前局面重新结算：黑棋获胜时累计黑胜加一。
    if (result == 2) ++score.whiteWins; // 按当前局面重新结算：白棋获胜时累计白胜加一。
    if (result == 3) ++score.draws; // 按当前局面重新结算：满盘无赢家时累计和棋加一。
    score.countedResult = result; // 记录本局现在已经计入的结果，下次调用用它判断是否重复或需要撤销。
}

void startLocalGame(GameState& game, LocalScore& score) // 函数入口：清空当前棋盘及本局结算标记，保留累计黑胜、白胜和和棋次数。
{ // 开始上方函数、条件、循环或类型的作用域。
    game = GameState(); // 更新数据：`game = GameState()`；赋值后的状态供后续逻辑或绘图使用。
    score.countedResult = 0; // 开始新局，只清空本局结算标记，累计胜场不会清零。
}

bool localRestartClick(int x, int y) // 函数入口：检查再来一局按钮位置；调用者另检查是否已经结束。
{ // 开始上方函数、条件、循环或类型的作用域。
    return insideButton(x, y, 560, 490, 210, 44); // 返回 `insideButton(x, y, 560, 490, 210, 44)`，将结果交给调用者。
}

bool localUndoClick(int x, int y) // 函数入口：检查本地悔棋按钮位置。
{ // 开始上方函数、条件、循环或类型的作用域。
    return insideButton(x, y, 560, 364, 210, 48); // 返回 `insideButton(x, y, 560, 364, 210, 48)`，将结果交给调用者。
}

bool localSaveClick(int x, int y) // 函数入口：检查本地保存棋谱按钮位置。
{ // 开始上方函数、条件、循环或类型的作用域。
    return insideButton(x, y, 560, 300, 210, 48); // 返回 `insideButton(x, y, 560, 300, 210, 48)`，将结果交给调用者。
}

bool localMove(GameState& game, int x, int y) // 函数入口：将点击转换为行列并调用共用落子规则。
{ // 开始上方函数、条件、循环或类型的作用域。
    int row, col; // 声明 row：棋盘行号；参数或长度由本行给出。
    return mouseToBoard(x, y, row, col) && placePiece(game, row, col); // 先检查鼠标是否在棋盘点击范围；成功才落子，利用 && 的短路避免使用无效行列。
}

void drawLocalGame(const GameState& game, const LocalScore& score, int mouseX, int mouseY) // 函数入口：绘制本地棋盘、黑色大字体计数和按状态出现的功能按钮。
{ // 开始上方函数、条件、循环或类型的作用域。
    drawMatch(game, L"本地对决", L"同一台电脑轮流落子", // 绘制共用棋盘页；canMove 只控制准星，实际合法性由落子函数检查。
              matchStatus(game), true, mouseX, mouseY); // 根据赢家和满盘状态返回胜负、和棋或轮次文字。
    const std::wstring counts = L"黑胜 " + std::to_wstring(score.blackWins) + // 声明 counts：拼接后的胜负计数字符串；按右侧表达式初始化。
        L"  白胜 " + std::to_wstring(score.whiteWins) + L"  和 " + std::to_wstring(score.draws); // 执行 `L"  白胜 " + std::to_wstring(score.whiteWins) + L"  和 " + std::to_wstring(score.draws)`；调用相应对象的方法完成本步骤。
    ege::setfont(24, 0, L"微软雅黑"); // 胜负计数使用24像素微软雅黑，比原来的16像素更易读。
    ege::setcolor(EGERGB(0, 0, 0)); // 将胜负计数字体明确设置为纯黑，避免继承按钮绘制后的白色。
    ege::setbkmode(TRANSPARENT); // 绘制计数时不盖住木纹背景。
    ege::outtextxy(565, 270, counts.c_str()); // 在指定坐标绘制文字；参数为 `565, 270, counts.c_str())`。
    if (!game.moves.empty()) // 检查 `if (!game.moves.empty())`；条件成立时执行括号之后或下一行的处理。
    { // 开始上方函数、条件、循环或类型的作用域。
        drawButton(560, 300, 210, 48, L"保存棋谱", mouseX, mouseY); // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
        drawButton(560, 364, 210, 48, L"悔棋", mouseX, mouseY); // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
    }
    if (localFinished(game)) // 检查 `if (localFinished(game))`；条件成立时执行括号之后或下一行的处理。
        drawButton(560, 490, 210, 44, L"再来一局", mouseX, mouseY); // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
}
