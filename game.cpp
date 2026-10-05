#include "game.h"
bool mouseToBoard(int mouseX, int mouseY, int& row, int& col)
{
    // 最外侧交点向外允许点击半格，便于点击边缘棋子的位置。
    const int halfCell = CELL_SIZE / 2; // 半格是 20 像素。
    const int length = (BOARD_SIZE - 1) * CELL_SIZE;
    // || 表示“或者”：任何一个方向越界都不落子，同时避免数组下标越界。
    // 右侧和下侧边界不包含，确保后面计算出的行列编号最大为 12。
    if (mouseX < BOARD_LEFT - halfCell || mouseX >= BOARD_LEFT + length + halfCell ||
        mouseY < BOARD_TOP - halfCell || mouseY >= BOARD_TOP + length + halfCell)
        return false; // 超出允许点击的区域，直接结束函数。

    // 减去棋盘起点得到相对坐标，加半格再做整数除法，把位置归到最近的交点。
    // 例如 mouseX = 66：(66 - 28 + 20) / 40 = 1，落在第 1 列（x = 68）。
    // 整数除法会舍去小数；恰好在两交点中间时归到右侧或下侧交点。
    col = (mouseX - BOARD_LEFT + halfCell) / CELL_SIZE;
    row = (mouseY - BOARD_TOP + halfCell) / CELL_SIZE;
    return true;
}

bool placePiece(GameState& game, int row, int col)
{
    if (game.winner != EMPTY) // 对局获胜后不再接受落子。
        return false;
    // 游戏逻辑独立检查边界，不依赖鼠标或绘图函数。
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
        return false;
    if (game.board[row][col] != EMPTY) // 交点上已经有棋子。
        return false; // 不覆盖原棋子，也不改变当前玩家或棋步记录。

    // Move{...} 创建一步棋的记录，push_back 将它追加到棋步列表末尾。
    game.moves.push_back(Move{row, col, game.currentPlayer});
    game.board[row][col] = game.currentPlayer; // 更新棋盘数据，记住这个位置已被占用。
    if (checkWin(game, row, col))
    {
        game.winner = game.currentPlayer; // 这里只记录结果，由 main 显示画面后弹窗。
        return true;
    }
    // 成功落子且未获胜时才换人：黑棋换成白棋，白棋换成黑棋。
    game.currentPlayer = game.currentPlayer == BLACK_PIECE ? WHITE_PIECE : BLACK_PIECE;
    return true; // 告诉调用者：本次落子成功。
}

bool checkWin(const GameState& game, int row, int col)
{
    if (row < 0 || row >= BOARD_SIZE ||
        col < 0 || col >= BOARD_SIZE)
        return false;

    const Piece piece = game.board[row][col];
    if (piece == EMPTY)
        return false;

    const int directions[4][2] = {
        {0, 1},
        {1, 0},
        {1, 1},
        {1, -1}
    };

    for (int i = 0; i < 4; i++)
    {
        int count = 1; 

        // 沿当前方向向两边查找。
        for (int sign = -1; sign <= 1; sign += 2)
        {
            const int dr = directions[i][0] * sign;
            const int dc = directions[i][1] * sign;
            int r = row + dr;
            int c = col + dc;

            while (r >= 0 && r < BOARD_SIZE &&
                   c >= 0 && c < BOARD_SIZE &&
                   game.board[r][c] == piece)
            {
                count++;
                r += dr;
                c += dc;
            }
        }

        if (count >= 5)
            return true;
    }

    return false;
}
