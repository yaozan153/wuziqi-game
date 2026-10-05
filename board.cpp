#include "board.h" 
#include <ege.h> 
#include <string>
using namespace ege;

namespace
{
    PIMAGE blackImage = NULL;
    PIMAGE whiteImage = NULL;
    PIMAGE boardImage = NULL;
    const int boardPadding = CELL_SIZE / 2; // 木纹向最外侧交点之外延伸半格。

    // 从程序所在目录找素材，避免从不同工作目录运行时找不到图片。
    bool loadImage(PIMAGE image, const wchar_t* name, int size = 0)
    {
        wchar_t executable[MAX_PATH] = {};
        GetModuleFileNameW(NULL, executable, MAX_PATH);
        std::wstring directory(executable);
        directory = directory.substr(0, directory.find_last_of(L"\\/"));
        std::wstring path = directory + L"\\assets\\" + name;
        return getimage(image, path.c_str(), size, size) == 0;
    }
}

bool loadPieceImages()
{
    blackImage = newimage();
    whiteImage = newimage();
    boardImage = newimage();
    const int textureSize = (BOARD_SIZE - 1) * CELL_SIZE + 2 * boardPadding;
    return loadImage(blackImage, L"black-stone.png") &&
           loadImage(whiteImage, L"white-stone.png") &&
           loadImage(boardImage, L"board-wood.png", textureSize);
}

void freePieceImages()
{
    if (blackImage) delimage(blackImage);
    if (whiteImage) delimage(whiteImage);
    if (boardImage) delimage(boardImage);
    blackImage = whiteImage = boardImage = NULL;
}

void drawBoard()
{
    
    const int boardLength = (BOARD_SIZE - 1) * CELL_SIZE;

    // 先铺木纹，再画网格；棋子在落子时画到网格上面。
    putimage(BOARD_LEFT - boardPadding, BOARD_TOP - boardPadding, boardImage);

    setcolor(EGERGB(30, 30, 30)); 

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        const int x = BOARD_LEFT + i * CELL_SIZE; 
        const int y = BOARD_TOP + i * CELL_SIZE; 

        line(BOARD_LEFT, y, BOARD_LEFT + boardLength, y); // 横线两端的 y 相同。
        line(x, BOARD_TOP, x, BOARD_TOP + boardLength); // 竖线两端的 x 相同。
    }

    // 星位坐标从 1 开始计数，换算像素位置时减 1。
    const int starPoints[5][2] = {{4, 4}, {4, 10}, {10, 4}, {10, 10}, {7, 7}};
    setfillcolor(EGERGB(30, 30, 30));
    for (int i = 0; i < 5; i++)
    {
        const int x = BOARD_LEFT + (starPoints[i][0] - 1) * CELL_SIZE;
        const int y = BOARD_TOP + (starPoints[i][1] - 1) * CELL_SIZE;
        fillellipse(x, y, 3, 3); // 两个半径均为 3，画出实心圆点。
    }
}

void drawPiece(int row, int col, Piece piece)
{
    
    const int x = BOARD_LEFT + col * CELL_SIZE;
    const int y = BOARD_TOP + row * CELL_SIZE;
    // 条件 ? 值一 : 值二 是三目运算符：这里按棋子颜色选择对应图片。
    // 两张图片均为 36×36 像素，中心对齐交点，透明区域不会遮挡棋盘。
    PIMAGE image = piece == BLACK_PIECE ? blackImage : whiteImage;
    putimage_withalpha(NULL, image, x - 18, y - 18);
}

void drawCrosshair(int row, int col)
{
    const int x = BOARD_LEFT + col * CELL_SIZE;
    const int y = BOARD_TOP + row * CELL_SIZE;
    const int halfSize = 22; // 整体宽高为 44，微调大小可以改这里。
    const int armLength = 14; // 每条短线的长度。
    const int inner = halfSize - armLength;
    setcolor(EGERGB(0, 150, 255));
    setlinewidth(2);
    // 四个直角的顶点靠近中心，短线向外伸出，与参考图方向一致。
    for (int dx = -1; dx <= 1; dx += 2)
        for (int dy = -1; dy <= 1; dy += 2)
        {
            line(x + dx * inner, y + dy * inner,
                 x + dx * halfSize, y + dy * inner);
            line(x + dx * inner, y + dy * inner,
                 x + dx * inner, y + dy * halfSize);
        }
    setlinewidth(1); // 恢复线宽，避免下一帧的棋盘线变粗。
}

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

bool placePiece(GameState& game, int mouseX, int mouseY)
{
    int row, col;
    if (!mouseToBoard(mouseX, mouseY, row, col))
        return false;
    if (game.board[row][col] != EMPTY) // 交点上已经有棋子。
        return false; // 不覆盖原棋子，也不改变当前玩家或棋步记录。

    // Move{...} 创建一步棋的记录，push_back 将它追加到棋步列表末尾。
    game.moves.push_back(Move{row, col, game.currentPlayer});
    game.board[row][col] = game.currentPlayer; // 更新棋盘数据，记住这个位置已被占用。
    drawPiece(row, col, game.currentPlayer); // 把这枚棋子画到窗口中。
    // 成功落子后才换人：黑棋换成白棋，白棋换成黑棋。
    game.currentPlayer = game.currentPlayer == BLACK_PIECE ? WHITE_PIECE : BLACK_PIECE;
    return true; // 告诉调用者：本次落子成功。
}
