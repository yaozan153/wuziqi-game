// 文件职责：自动化验证：断言规则、回合、文件或网络行为符合预期；不参与游戏主程序。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#include "../modes/human_ai/human_ai.h" // 引入 "../modes/human_ai/human_ai.h"，提供本文件使用的类型和函数声明。
#include "../board.h"
#include <cassert> // 引入 <cassert>，提供本文件使用的类型和函数声明。
#include <iostream> // 引入 <iostream>，提供本文件使用的类型和函数声明。

int main() // 独立测试入口：验证玩家与电脑轮次、选白首手和结束后拒绝落子；不链接进游戏主程序。
{ // 开始上方函数、条件、循环或类型的作用域。
    GameState black; // 声明 black：本行使用的局部数据；参数或长度由本行给出。
    assert(!computerAiMove(black, BLACK_PIECE)); // 测试断言：要求 `!computerAiMove(black, BLACK_PIECE` 成立；失败立即终止测试。
    assert(humanAiMove(black, BLACK_PIECE, BOARD_LEFT, BOARD_TOP)); // 测试断言：要求 `humanAiMove(black, BLACK_PIECE, BOARD_LEFT, BOARD_TOP` 成立；失败立即终止测试。
    assert(!humanAiMove(black, BLACK_PIECE, BOARD_LEFT + CELL_SIZE, BOARD_TOP)); // 测试断言：要求 `!humanAiMove(black, BLACK_PIECE, BOARD_LEFT + CELL_SIZE, BOARD_TOP` 成立；失败立即终止测试。
    assert(computerAiMove(black, BLACK_PIECE)); // 测试断言：要求 `computerAiMove(black, BLACK_PIECE` 成立；失败立即终止测试。
    assert(black.moves.size() == 2 && black.currentPlayer == BLACK_PIECE); // 测试断言：要求 `black.moves.size() == 2 && black.currentPlayer == BLACK_PIECE` 成立；失败立即终止测试。
    assert(!computerAiMove(black, BLACK_PIECE)); // 测试断言：要求 `!computerAiMove(black, BLACK_PIECE` 成立；失败立即终止测试。
    assert(!humanAiMove(black, BLACK_PIECE, BOARD_LEFT, BOARD_TOP)); // 测试断言：要求 `!humanAiMove(black, BLACK_PIECE, BOARD_LEFT, BOARD_TOP` 成立；失败立即终止测试。
    assert(!humanAiMove(black, BLACK_PIECE, -100, -100)); // 测试断言：要求 `!humanAiMove(black, BLACK_PIECE, -100, -100` 成立；失败立即终止测试。

    GameState white; // 声明 white：本行使用的局部数据；参数或长度由本行给出。
    assert(!humanAiMove(white, WHITE_PIECE, BOARD_LEFT, BOARD_TOP)); // 测试断言：要求 `!humanAiMove(white, WHITE_PIECE, BOARD_LEFT, BOARD_TOP` 成立；失败立即终止测试。
    assert(computerAiMove(white, WHITE_PIECE)); // 测试断言：要求 `computerAiMove(white, WHITE_PIECE` 成立；失败立即终止测试。
    assert(white.board[6][6] == BLACK_PIECE && white.currentPlayer == WHITE_PIECE); // 测试断言：要求 `white.board[6][6] == BLACK_PIECE && white.currentPlayer == WHITE_PIECE` 成立；失败立即终止测试。
    assert(humanAiMove(white, WHITE_PIECE, BOARD_LEFT, BOARD_TOP)); // 测试断言：要求 `humanAiMove(white, WHITE_PIECE, BOARD_LEFT, BOARD_TOP` 成立；失败立即终止测试。
    assert(computerAiMove(white, WHITE_PIECE)); // 测试断言：要求 `computerAiMove(white, WHITE_PIECE` 成立；失败立即终止测试。
    assert(white.moves.size() == 3 && white.currentPlayer == WHITE_PIECE); // 测试断言：要求 `white.moves.size() == 3 && white.currentPlayer == WHITE_PIECE` 成立；失败立即终止测试。

    white.winner = WHITE_PIECE; // 更新数据：`white.winner = WHITE_PIECE`；赋值后的状态供后续逻辑或绘图使用。
    assert(!humanAiMove(white, WHITE_PIECE, BOARD_LEFT + CELL_SIZE, BOARD_TOP)); // 测试断言：要求 `!humanAiMove(white, WHITE_PIECE, BOARD_LEFT + CELL_SIZE, BOARD_TOP` 成立；失败立即终止测试。
    assert(!computerAiMove(white, WHITE_PIECE)); // 测试断言：要求 `!computerAiMove(white, WHITE_PIECE` 成立；失败立即终止测试。
    assert(!computerAiMove(black, EMPTY)); // 测试断言：要求 `!computerAiMove(black, EMPTY` 成立；失败立即终止测试。
    assert(!humanAiMove(black, EMPTY, BOARD_LEFT, BOARD_TOP)); // 测试断言：要求 `!humanAiMove(black, EMPTY, BOARD_LEFT, BOARD_TOP` 成立；失败立即终止测试。

    GameState full; // 声明 full：棋盘是否已经下满；参数或长度由本行给出。
    for (int r = 0; r < BOARD_SIZE; ++r) // 循环推进：`int r = 0; r < BOARD_SIZE; ++r)`；逐项处理棋格、方向、字符或列表元素。
        for (int c = 0; c < BOARD_SIZE; ++c) // 循环推进：`int c = 0; c < BOARD_SIZE; ++c)`；逐项处理棋格、方向、字符或列表元素。
        { // 开始上方函数、条件、循环或类型的作用域。
            full.board[r][c] = BLACK_PIECE; // 更新数据：`full.board[r][c] = BLACK_PIECE`；赋值后的状态供后续逻辑或绘图使用。
            full.moves.push_back(Move{r, c, BLACK_PIECE}); // 追加一项到 `full.moves`，保留数据的先后顺序。
        }
    assert(!computerAiMove(full, WHITE_PIECE)); // 测试断言：要求 `!computerAiMove(full, WHITE_PIECE` 成立；失败立即终止测试。
    assert(full.moves.size() == BOARD_SIZE * BOARD_SIZE); // 测试断言：要求 `full.moves.size() == BOARD_SIZE * BOARD_SIZE` 成立；失败立即终止测试。
    std::cout << "Human/AI integration tests passed\n"; // 输出测试执行结果，便于确认测试是否运行到末尾。
}
