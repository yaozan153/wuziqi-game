// 文件职责：自动化验证：断言规则、回合、文件或网络行为符合预期；不参与游戏主程序。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#include "../game.h" // 引入 "../game.h"，提供本文件使用的类型和函数声明。
#include <cassert> // 引入 <cassert>，提供本文件使用的类型和函数声明。
#include <iostream> // 引入 <iostream>，提供本文件使用的类型和函数声明。

int main() // 独立测试入口：验证空局悔棋、回合恢复和获胜后悔棋；不链接进游戏主程序。
{ // 开始上方函数、条件、循环或类型的作用域。
    GameState game; // 声明 game：当前棋局；参数或长度由本行给出。
    assert(!undoLastMove(game)); // 测试断言：要求 `!undoLastMove(game` 成立；失败立即终止测试。
    assert(game.currentPlayer == BLACK_PIECE && game.winner == EMPTY); // 测试断言：要求 `game.currentPlayer == BLACK_PIECE && game.winner == EMPTY` 成立；失败立即终止测试。
    assert(placePiece(game, 6, 6)); // 测试断言：要求 `placePiece(game, 6, 6` 成立；失败立即终止测试。
    assert(placePiece(game, 6, 7)); // 测试断言：要求 `placePiece(game, 6, 7` 成立；失败立即终止测试。
    assert(undoLastMove(game)); // 测试断言：要求 `undoLastMove(game` 成立；失败立即终止测试。
    assert(game.moves.size() == 1 && game.board[6][7] == EMPTY); // 测试断言：要求 `game.moves.size() == 1 && game.board[6][7] == EMPTY` 成立；失败立即终止测试。
    assert(game.board[6][6] == BLACK_PIECE && game.currentPlayer == WHITE_PIECE); // 测试断言：要求 `game.board[6][6] == BLACK_PIECE && game.currentPlayer == WHITE_PIECE` 成立；失败立即终止测试。
    assert(placePiece(game, 7, 7)); // 测试断言：要求 `placePiece(game, 7, 7` 成立；失败立即终止测试。
    assert(game.board[7][7] == WHITE_PIECE); // 测试断言：要求 `game.board[7][7] == WHITE_PIECE` 成立；失败立即终止测试。
    assert(undoLastMove(game) && undoLastMove(game)); // 测试断言：要求 `undoLastMove(game) && undoLastMove(game` 成立；失败立即终止测试。
    assert(game.moves.empty() && game.currentPlayer == BLACK_PIECE); // 测试断言：要求 `game.moves.empty() && game.currentPlayer == BLACK_PIECE` 成立；失败立即终止测试。
    assert(!undoLastMove(game)); // 测试断言：要求 `!undoLastMove(game` 成立；失败立即终止测试。

    for (int col = 0; col < 5; ++col) // 循环推进：`int col = 0; col < 5; ++col)`；逐项处理棋格、方向、字符或列表元素。
    { // 开始上方函数、条件、循环或类型的作用域。
        assert(placePiece(game, 0, col)); // 测试断言：要求 `placePiece(game, 0, col` 成立；失败立即终止测试。
        if (col < 4) // 检查 `if (col < 4)`；条件成立时执行括号之后或下一行的处理。
            assert(placePiece(game, 2, col)); // 测试断言：要求 `placePiece(game, 2, col` 成立；失败立即终止测试。
    }
    assert(game.winner == BLACK_PIECE); // 测试断言：要求 `game.winner == BLACK_PIECE` 成立；失败立即终止测试。
    assert(undoLastMove(game)); // 测试断言：要求 `undoLastMove(game` 成立；失败立即终止测试。
    assert(game.winner == EMPTY && game.currentPlayer == BLACK_PIECE); // 测试断言：要求 `game.winner == EMPTY && game.currentPlayer == BLACK_PIECE` 成立；失败立即终止测试。
    assert(game.moves.size() == 8 && game.board[0][4] == EMPTY); // 测试断言：要求 `game.moves.size() == 8 && game.board[0][4] == EMPTY` 成立；失败立即终止测试。
    assert(placePiece(game, 0, 4) && game.winner == BLACK_PIECE); // 测试断言：要求 `placePiece(game, 0, 4) && game.winner == BLACK_PIECE` 成立；失败立即终止测试。
    std::cout << "Undo tests passed\n"; // 输出测试执行结果，便于确认测试是否运行到末尾。
}
