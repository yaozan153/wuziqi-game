// 文件职责：自动化验证：断言规则、回合、文件或网络行为符合预期；不参与游戏主程序。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#include "../modes/local/local.h" // 引入 "../modes/local/local.h"，提供本文件使用的类型和函数声明。
#include <cassert> // 引入 <cassert>，提供本文件使用的类型和函数声明。
#include <iostream> // 引入 <iostream>，提供本文件使用的类型和函数声明。

int main() // 独立测试入口：验证胜负不重复、悔棋撤销、本局重赛与满盘和棋计数；不链接进游戏主程序。
{ // 开始上方函数、条件、循环或类型的作用域。
    LocalScore score; // 声明 score：评分或本地统计对象；参数或长度由本行给出。
    GameState game; // 声明 game：当前棋局；参数或长度由本行给出。
    updateLocalScore(game, score); // 先撤销旧结算再登记新结果，重复调用不重复加分，悔棋能撤销结果。
    assert(score.blackWins == 0 && score.whiteWins == 0 && score.draws == 0); // 测试断言：要求 `score.blackWins == 0 && score.whiteWins == 0 && score.draws == 0` 成立；失败立即终止测试。
    for (int c = 0; c < 5; ++c) // 循环推进：`int c = 0; c < 5; ++c)`；逐项处理棋格、方向、字符或列表元素。
    { // 开始上方函数、条件、循环或类型的作用域。
        assert(placePiece(game, 0, c)); // 测试断言：要求 `placePiece(game, 0, c` 成立；失败立即终止测试。
        updateLocalScore(game, score); // 先撤销旧结算再登记新结果，重复调用不重复加分，悔棋能撤销结果。
        if (c < 4) assert(placePiece(game, 2, c)); // 检查 `if (c < 4) assert(placePiece(game, 2, c))`；条件成立时执行括号之后或下一行的处理。
    }
    assert(score.blackWins == 1 && localFinished(game)); // 测试断言：要求 `score.blackWins == 1 && localFinished(game` 成立；失败立即终止测试。
    updateLocalScore(game, score); // 先撤销旧结算再登记新结果，重复调用不重复加分，悔棋能撤销结果。
    assert(score.blackWins == 1); // 测试断言：要求 `score.blackWins == 1` 成立；失败立即终止测试。
    assert(undoLastMove(game)); // 测试断言：要求 `undoLastMove(game` 成立；失败立即终止测试。
    updateLocalScore(game, score); // 先撤销旧结算再登记新结果，重复调用不重复加分，悔棋能撤销结果。
    assert(score.blackWins == 0 && !localFinished(game)); // 测试断言：要求 `score.blackWins == 0 && !localFinished(game` 成立；失败立即终止测试。
    assert(placePiece(game, 0, 4)); // 测试断言：要求 `placePiece(game, 0, 4` 成立；失败立即终止测试。
    updateLocalScore(game, score); // 先撤销旧结算再登记新结果，重复调用不重复加分，悔棋能撤销结果。
    assert(score.blackWins == 1); // 测试断言：要求 `score.blackWins == 1` 成立；失败立即终止测试。
    startLocalGame(game, score); // 清空当前棋盘及本局结算标记，保留累计黑胜、白胜和和棋次数。
    updateLocalScore(game, score); // 先撤销旧结算再登记新结果，重复调用不重复加分，悔棋能撤销结果。
    assert(score.blackWins == 1 && game.moves.empty()); // 测试断言：要求 `score.blackWins == 1 && game.moves.empty(` 成立；失败立即终止测试。

    for (int c = 0; c < 5; ++c) // 循环推进：`int c = 0; c < 5; ++c)`；逐项处理棋格、方向、字符或列表元素。
    { // 开始上方函数、条件、循环或类型的作用域。
        assert(placePiece(game, 4, c * 2)); // 测试断言：要求 `placePiece(game, 4, c * 2` 成立；失败立即终止测试。
        assert(placePiece(game, 0, c)); // 测试断言：要求 `placePiece(game, 0, c` 成立；失败立即终止测试。
        updateLocalScore(game, score); // 先撤销旧结算再登记新结果，重复调用不重复加分，悔棋能撤销结果。
    }
    assert(score.whiteWins == 1 && score.blackWins == 1); // 测试断言：要求 `score.whiteWins == 1 && score.blackWins == 1` 成立；失败立即终止测试。
    startLocalGame(game, score); // 清空当前棋盘及本局结算标记，保留累计黑胜、白胜和和棋次数。
    for (int r = 0; r < BOARD_SIZE; ++r) // 循环推进：`int r = 0; r < BOARD_SIZE; ++r)`；逐项处理棋格、方向、字符或列表元素。
        for (int c = 0; c < BOARD_SIZE; ++c) // 循环推进：`int c = 0; c < BOARD_SIZE; ++c)`；逐项处理棋格、方向、字符或列表元素。
        { // 开始上方函数、条件、循环或类型的作用域。
            const PieceColor piece = (r + 2 * c) % 4 < 2 ? BLACK_PIECE : WHITE_PIECE; // 声明 piece：棋子颜色；按右侧表达式初始化。
            game.board[r][c] = piece; // 更新数据：`game.board[r][c] = piece`；赋值后的状态供后续逻辑或绘图使用。
            game.moves.push_back(Move{r, c, piece}); // 追加一项到 `game.moves`，保留数据的先后顺序。
        }
    for (int r = 0; r < BOARD_SIZE; ++r) // 循环推进：`int r = 0; r < BOARD_SIZE; ++r)`；逐项处理棋格、方向、字符或列表元素。
        for (int c = 0; c < BOARD_SIZE; ++c) // 循环推进：`int c = 0; c < BOARD_SIZE; ++c)`；逐项处理棋格、方向、字符或列表元素。
            assert(!checkWin(game, r, c)); // 测试断言：要求 `!checkWin(game, r, c` 成立；失败立即终止测试。
    updateLocalScore(game, score); // 先撤销旧结算再登记新结果，重复调用不重复加分，悔棋能撤销结果。
    updateLocalScore(game, score); // 先撤销旧结算再登记新结果，重复调用不重复加分，悔棋能撤销结果。
    assert(score.draws == 1); // 测试断言：要求 `score.draws == 1` 成立；失败立即终止测试。
    assert(undoLastMove(game)); // 测试断言：要求 `undoLastMove(game` 成立；失败立即终止测试。
    updateLocalScore(game, score); // 先撤销旧结算再登记新结果，重复调用不重复加分，悔棋能撤销结果。
    assert(score.draws == 0); // 测试断言：要求 `score.draws == 0` 成立；失败立即终止测试。
    const Move last{12, 12, BLACK_PIECE}; // 声明 last：本行使用的局部数据；参数或长度由本行给出。
    game.board[last.row][last.col] = last.piece; // 更新数据：`game.board[last.row][last.col] = last.piece`；赋值后的状态供后续逻辑或绘图使用。
    game.moves.push_back(last); // 追加一项到 `game.moves`，保留数据的先后顺序。
    game.winner = BLACK_PIECE; // 更新数据：`game.winner = BLACK_PIECE`；赋值后的状态供后续逻辑或绘图使用。
    updateLocalScore(game, score); // 先撤销旧结算再登记新结果，重复调用不重复加分，悔棋能撤销结果。
    assert(score.blackWins == 2 && score.draws == 0); // 测试断言：要求 `score.blackWins == 2 && score.draws == 0` 成立；失败立即终止测试。
    startLocalGame(game, score); // 清空当前棋盘及本局结算标记，保留累计黑胜、白胜和和棋次数。
    assert(placePiece(game, 0, 0)); // 测试断言：要求 `placePiece(game, 0, 0` 成立；失败立即终止测试。
    startLocalGame(game, score); // 清空当前棋盘及本局结算标记，保留累计黑胜、白胜和和棋次数。
    assert(score.blackWins == 2 && score.whiteWins == 1 && score.draws == 0); // 测试断言：要求 `score.blackWins == 2 && score.whiteWins == 1 && score.draws == 0` 成立；失败立即终止测试。
    assert(localRestartClick(600, 510) && !localRestartClick(600, 440)); // 测试断言：要求 `localRestartClick(600, 510) && !localRestartClick(600, 440` 成立；失败立即终止测试。
    std::cout << "Local score tests passed\n"; // 输出测试执行结果，便于确认测试是否运行到末尾。
}
