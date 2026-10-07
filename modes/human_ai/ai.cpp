// 文件职责：单步启发式 AI：取胜优先，其次挡对手取胜，再比较攻防棋形评分；没有多步搜索。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#include "ai.h" // 引入 "ai.h"，提供本文件使用的类型和函数声明。
#include <cstdlib> // 引入 <cstdlib>，提供本文件使用的类型和函数声明。

namespace // 开启匿名命名空间，让内部辅助函数只在当前源文件可见。
{ // 开始上方函数、条件、循环或类型的作用域。
    const int DIRECTIONS[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}}; // 声明 DIRECTIONS：本行使用的局部数据；按右侧表达式初始化。
    const int WIN_SCORE = 1000000; // 声明 WIN_SCORE：本行使用的局部数据；按右侧表达式初始化。

    bool inside(int row, int col) // 函数入口：检查 AI 计算的行列是否仍在棋盘范围内。
    { // 开始上方函数、条件、循环或类型的作用域。
        return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE; // 返回 `row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE`，将结果交给调用者。
    }

    // 假设在空位落下 piece，计算穿过该点的棋形，无须修改棋盘。
    int evaluatePoint(const GameState& game, int row, int col, PieceColor piece) // 函数入口：假设候选空位放指定颜色，按连子、两端开口及五格窗口计算棋形得分。
    { // 开始上方函数、条件、循环或类型的作用域。
        int score = 0; // 声明 score：评分或本地统计对象；按右侧表达式初始化。
        for (int direction = 0; direction < 4; ++direction) // 循环推进：`int direction = 0; direction < 4; ++direction)`；逐项处理棋格、方向、字符或列表元素。
        { // 开始上方函数、条件、循环或类型的作用域。
            const int dr = DIRECTIONS[direction][0]; // 声明 dr：当前方向行增量；按右侧表达式初始化。
            const int dc = DIRECTIONS[direction][1]; // 声明 dc：当前方向列增量；按右侧表达式初始化。
            int count = 1; // 声明 count：连续棋子数或复盘进度文字；按右侧表达式初始化。
            int openEnds = 0; // 声明 openEnds：连续棋形两端的可延伸空位数；按右侧表达式初始化。
            for (int sign = -1; sign <= 1; sign += 2) // 循环推进：`int sign = -1; sign <= 1; sign += 2)`；逐项处理棋格、方向、字符或列表元素。
            { // 开始上方函数、条件、循环或类型的作用域。
                int r = row + sign * dr; // 声明 r：计算中的行号；按右侧表达式初始化。
                int c = col + sign * dc; // 声明 c：计算中的列号；按右侧表达式初始化。
                while (inside(r, c) && game.board[r][c] == piece) // 只要 `inside(r, c) && game.board[r][c] == piece)` 成立就继续处理；用于事件、连子或网络队列。
                { // 开始上方函数、条件、循环或类型的作用域。
                    ++count; // 递增或递减 `++count`，推进计数、坐标或光标。
                    r += sign * dr; // 更新数据：`r += sign * dr`；赋值后的状态供后续逻辑或绘图使用。
                    c += sign * dc; // 更新数据：`c += sign * dc`；赋值后的状态供后续逻辑或绘图使用。
                }
                if (inside(r, c) && game.board[r][c] == EMPTY) // 检查 `if (inside(r, c) && game.board[r][c] == EMPTY)`；条件成立时执行括号之后或下一行的处理。
                    ++openEnds; // 递增或递减 `++openEnds`，推进计数、坐标或光标。
            }
            if (count >= 5) // 连续五颗或更多都算赢；这里没有使用恰好等于五的规则。
                return WIN_SCORE; // 返回 `WIN_SCORE`，将结果交给调用者。

            // 活四、冲四、活三等；边界与对方棋子都视为封口。
            if (openEnds == 2) // 检查 `if (openEnds == 2)`；条件成立时执行括号之后或下一行的处理。
            { // 开始上方函数、条件、循环或类型的作用域。
                const int openScores[5] = {0, 10, 100, 1000, 10000}; // 声明 openScores：本行使用的局部数据；按右侧表达式初始化。
                score += openScores[count]; // 更新数据：`score += openScores[count]`；赋值后的状态供后续逻辑或绘图使用。
            }
            else if (openEnds == 1) // 前一条件不满足时，再检查 `(openEnds == 1)`。
            { // 开始上方函数、条件、循环或类型的作用域。
                const int closedScores[5] = {0, 1, 10, 100, 3000}; // 声明 closedScores：本行使用的局部数据；按右侧表达式初始化。
                score += closedScores[count]; // 更新数据：`score += closedScores[count]`；赋值后的状态供后续逻辑或绘图使用。
            }

            // 补充长度为 5 的窗口评分，识别含空隙的棋形，如 XX_XX。
            // 只统计包含当前候选点且没有对手棋子的窗口。
            for (int offset = -4; offset <= 0; ++offset) // 循环推进：`int offset = -4; offset <= 0; ++offset)`；逐项处理棋格、方向、字符或列表元素。
            { // 开始上方函数、条件、循环或类型的作用域。
                int stones = 0; // 声明 stones：五格窗口中己方棋子数；按右侧表达式初始化。
                bool usable = true; // 声明 usable：五格窗口是否在界内且没有对手棋子；按右侧表达式初始化。
                for (int step = 0; step < 5; ++step) // 循环推进：`int step = 0; step < 5; ++step)`；逐项处理棋格、方向、字符或列表元素。
                { // 开始上方函数、条件、循环或类型的作用域。
                    const int r = row + (offset + step) * dr; // 声明 r：计算中的行号；按右侧表达式初始化。
                    const int c = col + (offset + step) * dc; // 声明 c：计算中的列号；按右侧表达式初始化。
                    if (!inside(r, c)) // 检查 `if (!inside(r, c))`；条件成立时执行括号之后或下一行的处理。
                    { // 开始上方函数、条件、循环或类型的作用域。
                        usable = false; // 更新数据：`usable = false`；赋值后的状态供后续逻辑或绘图使用。
                        break; // 跳出当前循环，继续执行循环之后的代码。
                    }
                    const PieceColor cell = (r == row && c == col) ? piece : game.board[r][c]; // 声明 cell：计算中观察的棋盘格；按右侧表达式初始化。
                    if (cell != EMPTY && cell != piece) // 检查 `if (cell != EMPTY && cell != piece)`；条件成立时执行括号之后或下一行的处理。
                    { // 开始上方函数、条件、循环或类型的作用域。
                        usable = false; // 更新数据：`usable = false`；赋值后的状态供后续逻辑或绘图使用。
                        break; // 跳出当前循环，继续执行循环之后的代码。
                    }
                    if (cell == piece) // 检查 `if (cell == piece)`；条件成立时执行括号之后或下一行的处理。
                        ++stones; // 递增或递减 `++stones`，推进计数、坐标或光标。
                }
                if (usable) // 检查 `if (usable)`；条件成立时执行括号之后或下一行的处理。
                { // 开始上方函数、条件、循环或类型的作用域。
                    const int windowScores[6] = {0, 0, 5, 50, 500, WIN_SCORE}; // 声明 windowScores：本行使用的局部数据；按右侧表达式初始化。
                    score += windowScores[stones]; // 更新数据：`score += windowScores[stones]`；赋值后的状态供后续逻辑或绘图使用。
                }
            }
        }
        return score; // 返回 `score`，将结果交给调用者。
    }
}

bool findBestAiMove(const GameState& game, PieceColor aiPiece, int& row, int& col) // 函数入口：遍历空位，比较必胜/必防优先级、攻防得分和距中心距离，输出最佳行列。
{ // 开始上方函数、条件、循环或类型的作用域。
    row = col = -1; // 更新数据：`row = col = -1`；赋值后的状态供后续逻辑或绘图使用。
    if (game.winner != EMPTY || (aiPiece != BLACK_PIECE && aiPiece != WHITE_PIECE)) // 检查 `if (game.winner != EMPTY || (aiPiece != BLACK_PIECE && aiPiece != WHITE_PIECE))`；条件成立时执行括号之后或下一行的处理。
        return false; // 返回 false：本次条件不满足或操作未成功；具体含义见当前函数说明。

    const PieceColor opponent = aiPiece == BLACK_PIECE ? WHITE_PIECE : BLACK_PIECE; // 声明 opponent：对手棋色；按右侧表达式初始化。
    int bestPriority = -1; // 声明 bestPriority：目前最佳候选的优先级；按右侧表达式初始化。
    int bestScore = -1; // 声明 bestScore：目前最佳候选分数；按右侧表达式初始化。
    int bestDistance = BOARD_SIZE * BOARD_SIZE; // 声明 bestDistance：最佳候选距中心的距离；按右侧表达式初始化。
    for (int r = 0; r < BOARD_SIZE; ++r) // 循环推进：`int r = 0; r < BOARD_SIZE; ++r)`；逐项处理棋格、方向、字符或列表元素。
    { // 开始上方函数、条件、循环或类型的作用域。
        for (int c = 0; c < BOARD_SIZE; ++c) // 循环推进：`int c = 0; c < BOARD_SIZE; ++c)`；逐项处理棋格、方向、字符或列表元素。
        { // 开始上方函数、条件、循环或类型的作用域。
            if (game.board[r][c] != EMPTY) // 检查 `if (game.board[r][c] != EMPTY)`；条件成立时执行括号之后或下一行的处理。
                continue; // 跳过本轮剩余处理，继续下一条网络事件或循环元素。
            const int attack = evaluatePoint(game, r, c, aiPiece); // 声明 attack：AI 自己在候选点的进攻分；按右侧表达式初始化。
            const int defense = evaluatePoint(game, r, c, opponent); // 声明 defense：对手在候选点的威胁分；按右侧表达式初始化。
            // 用独立优先级保证进攻必胜点不会被防守分数覆盖。
            const int priority = attack == WIN_SCORE ? 2 : defense == WIN_SCORE ? 1 : 0; // 声明 priority：必胜高于必防的候选优先级；按右侧表达式初始化。
            const int score = attack * 2 + defense; // 声明 score：评分或本地统计对象；按右侧表达式初始化。
            const int distance = std::abs(r - BOARD_SIZE / 2) + std::abs(c - BOARD_SIZE / 2); // 声明 distance：候选点到棋盘中心的曼哈顿距离；按右侧表达式初始化。
            if (priority > bestPriority || // 检查 `if (priority > bestPriority ||`；条件成立时执行括号之后或下一行的处理。
                (priority == bestPriority && (score > bestScore || // 继续上方条件：`(priority == bestPriority && (score > bestScore ||`；&& 要求同时满足，|| 表示任一成立。
                 (score == bestScore && distance < bestDistance)))) // 承接上方表达式的参数、条件或初值：`(score == bestScore && distance < bestDistance))))`。
            { // 开始上方函数、条件、循环或类型的作用域。
                bestPriority = priority; // 更新数据：`bestPriority = priority`；赋值后的状态供后续逻辑或绘图使用。
                bestScore = score; // 更新数据：`bestScore = score`；赋值后的状态供后续逻辑或绘图使用。
                bestDistance = distance; // 更新数据：`bestDistance = distance`；赋值后的状态供后续逻辑或绘图使用。
                row = r; // 更新数据：`row = r`；赋值后的状态供后续逻辑或绘图使用。
                col = c; // 更新数据：`col = c`；赋值后的状态供后续逻辑或绘图使用。
            }
        }
    }
    return row != -1; // 返回 `row != -1`，将结果交给调用者。
}
