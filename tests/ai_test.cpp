// 文件职责：自动化验证：断言规则、回合、文件或网络行为符合预期；不参与游戏主程序。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#include "../modes/human_ai/ai.h" // 引入 "../modes/human_ai/ai.h"，提供本文件使用的类型和函数声明。
#include <cassert> // 引入 <cassert>，提供本文件使用的类型和函数声明。
#include <iostream> // 引入 <iostream>，提供本文件使用的类型和函数声明。

int main() // 独立测试入口：验证 AI 取胜、防守、空隙棋形和非法状态；不链接进游戏主程序。
{ // 开始上方函数、条件、循环或类型的作用域。
    int row, col; // 声明 row：棋盘行号；参数或长度由本行给出。
    GameState empty; // 声明 empty：本行使用的局部数据；参数或长度由本行给出。
    assert(findBestAiMove(empty, BLACK_PIECE, row, col)); // 测试断言：要求 `findBestAiMove(empty, BLACK_PIECE, row, col` 成立；失败立即终止测试。
    assert(row == 6 && col == 6); // 测试断言：要求 `row == 6 && col == 6` 成立；失败立即终止测试。
    assert(empty.board[row][col] == EMPTY && empty.moves.empty()); // 测试断言：要求 `empty.board[row][col] == EMPTY && empty.moves.empty(` 成立；失败立即终止测试。

    const int directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}}; // 声明 directions：横竖与两条斜线方向表；按右侧表达式初始化。
    for (int color = BLACK_PIECE; color <= WHITE_PIECE; ++color) // 循环推进：`int color = BLACK_PIECE; color <= WHITE_PIECE; ++color)`；逐项处理棋格、方向、字符或列表元素。
    { // 开始上方函数、条件、循环或类型的作用域。
        const PieceColor ai = static_cast<PieceColor>(color); // 声明 ai：本行使用的局部数据；按右侧表达式初始化。
        const PieceColor opponent = ai == BLACK_PIECE ? WHITE_PIECE : BLACK_PIECE; // 声明 opponent：对手棋色；按右侧表达式初始化。
        for (int d = 0; d < 4; ++d) // 循环推进：`int d = 0; d < 4; ++d)`；逐项处理棋格、方向、字符或列表元素。
        { // 开始上方函数、条件、循环或类型的作用域。
            for (int defensive = 0; defensive < 2; ++defensive) // 循环推进：`int defensive = 0; defensive < 2; ++defensive)`；逐项处理棋格、方向、字符或列表元素。
            { // 开始上方函数、条件、循环或类型的作用域。
                GameState game; // 声明 game：当前棋局；参数或长度由本行给出。
                const int dr = directions[d][0], dc = directions[d][1]; // 声明 dr：当前方向行增量；按右侧表达式初始化。
                const PieceColor line = defensive ? opponent : ai; // 声明 line：本行使用的局部数据；按右侧表达式初始化。
                game.board[5 - dr][5 - dc] = defensive ? ai : opponent; // 更新数据：`game.board[5 - dr][5 - dc] = defensive ? ai : opponent`；赋值后的状态供后续逻辑或绘图使用。
                for (int i = 0; i < 4; ++i) // 循环推进：`int i = 0; i < 4; ++i)`；逐项处理棋格、方向、字符或列表元素。
                    game.board[5 + i * dr][5 + i * dc] = line; // 更新数据：`game.board[5 + i * dr][5 + i * dc] = line`；赋值后的状态供后续逻辑或绘图使用。
                assert(findBestAiMove(game, ai, row, col)); // 测试断言：要求 `findBestAiMove(game, ai, row, col` 成立；失败立即终止测试。
                assert(row == 5 + 4 * dr && col == 5 + 4 * dc); // 测试断言：要求 `row == 5 + 4 * dr && col == 5 + 4 * dc` 成立；失败立即终止测试。
                assert(game.board[row][col] == EMPTY); // 测试断言：要求 `game.board[row][col] == EMPTY` 成立；失败立即终止测试。
                assert(game.moves.empty() && game.currentPlayer == BLACK_PIECE); // 测试断言：要求 `game.moves.empty() && game.currentPlayer == BLACK_PIECE` 成立；失败立即终止测试。
            }
        }
    }

    // 双方都能下一步获胜时，AI 应先赢，而不是去挡。
    GameState race; // 声明 race：本行使用的局部数据；参数或长度由本行给出。
    for (int c = 0; c < 4; ++c) // 循环推进：`int c = 0; c < 4; ++c)`；逐项处理棋格、方向、字符或列表元素。
    { // 开始上方函数、条件、循环或类型的作用域。
        race.board[0][c] = BLACK_PIECE; // 更新数据：`race.board[0][c] = BLACK_PIECE`；赋值后的状态供后续逻辑或绘图使用。
        race.board[12][c] = WHITE_PIECE; // 更新数据：`race.board[12][c] = WHITE_PIECE`；赋值后的状态供后续逻辑或绘图使用。
    }
    assert(findBestAiMove(race, BLACK_PIECE, row, col)); // 测试断言：要求 `findBestAiMove(race, BLACK_PIECE, row, col` 成立；失败立即终止测试。
    assert(row == 0 && col == 4); // 测试断言：要求 `row == 0 && col == 4` 成立；失败立即终止测试。

    // 内部空隙也必须识别为立即获胜点。
    GameState gap; // 声明 gap：本行使用的局部数据；参数或长度由本行给出。
    gap.board[6][3] = gap.board[6][4] = BLACK_PIECE; // 更新数据：`gap.board[6][3] = gap.board[6][4] = BLACK_PIECE`；赋值后的状态供后续逻辑或绘图使用。
    gap.board[6][6] = gap.board[6][7] = BLACK_PIECE; // 更新数据：`gap.board[6][6] = gap.board[6][7] = BLACK_PIECE`；赋值后的状态供后续逻辑或绘图使用。
    assert(findBestAiMove(gap, BLACK_PIECE, row, col)); // 测试断言：要求 `findBestAiMove(gap, BLACK_PIECE, row, col` 成立；失败立即终止测试。
    assert(row == 6 && col == 5); // 测试断言：要求 `row == 6 && col == 5` 成立；失败立即终止测试。

    assert(!findBestAiMove(empty, EMPTY, row, col)); // 测试断言：要求 `!findBestAiMove(empty, EMPTY, row, col` 成立；失败立即终止测试。
    assert(row == -1 && col == -1); // 测试断言：要求 `row == -1 && col == -1` 成立；失败立即终止测试。
    empty.winner = WHITE_PIECE; // 更新数据：`empty.winner = WHITE_PIECE`；赋值后的状态供后续逻辑或绘图使用。
    assert(!findBestAiMove(empty, BLACK_PIECE, row, col)); // 测试断言：要求 `!findBestAiMove(empty, BLACK_PIECE, row, col` 成立；失败立即终止测试。
    assert(row == -1 && col == -1); // 测试断言：要求 `row == -1 && col == -1` 成立；失败立即终止测试。

    GameState full; // 声明 full：棋盘是否已经下满；参数或长度由本行给出。
    for (int r = 0; r < BOARD_SIZE; ++r) // 循环推进：`int r = 0; r < BOARD_SIZE; ++r)`；逐项处理棋格、方向、字符或列表元素。
        for (int c = 0; c < BOARD_SIZE; ++c) // 循环推进：`int c = 0; c < BOARD_SIZE; ++c)`；逐项处理棋格、方向、字符或列表元素。
            full.board[r][c] = (r + c) % 2 ? BLACK_PIECE : WHITE_PIECE; // 更新数据：`full.board[r][c] = (r + c) % 2 ? BLACK_PIECE : WHITE_PIECE`；赋值后的状态供后续逻辑或绘图使用。
    assert(!findBestAiMove(full, WHITE_PIECE, row, col)); // 测试断言：要求 `!findBestAiMove(full, WHITE_PIECE, row, col` 成立；失败立即终止测试。
    assert(row == -1 && col == -1); // 测试断言：要求 `row == -1 && col == -1` 成立；失败立即终止测试。
    full.board[12][12] = EMPTY; // 更新数据：`full.board[12][12] = EMPTY`；赋值后的状态供后续逻辑或绘图使用。
    assert(findBestAiMove(full, WHITE_PIECE, row, col)); // 测试断言：要求 `findBestAiMove(full, WHITE_PIECE, row, col` 成立；失败立即终止测试。
    assert(row == 12 && col == 12); // 测试断言：要求 `row == 12 && col == 12` 成立；失败立即终止测试。
    std::cout << "AI tests passed\n"; // 输出测试执行结果，便于确认测试是否运行到末尾。
}
