#include "board.h" 
#include <ege.h> 
#include <string>
using namespace ege;

namespace
{
    using namespace std;
    PIMAGE blackImage = NULL;
    PIMAGE whiteImage = NULL;
    PIMAGE boardImage = NULL;
    const int boardPadding = CELL_SIZE / 2; // 木纹向最外侧交点之外延伸半格。

    // 从程序所在目录找素材，避免从不同工作目录运行时找不到图片。
    bool loadImage(PIMAGE image, const wchar_t* name, int size = 0)
    {
        wchar_t executable[MAX_PATH] = {};
        GetModuleFileNameW(NULL, executable, MAX_PATH);
        wstring directory(executable);
        directory = directory.substr(0, directory.find_last_of(L"\\/"));
        wstring path = directory + L"\\assets\\" + name;
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
    const int halfSize = 22; 
    const int armLength = 14; 
    const int inner = halfSize - armLength;
    setcolor(EGERGB(0, 150, 255));
    setlinestyle(SOLID_LINE, 0, 3); // 普通 line() 的线宽由第三个参数控制。
    // 四个直角的顶点靠近中心，短线向外伸出，与参考图方向一致。
    for (int dx = -1; dx <= 1; dx += 2)
        for (int dy = -1; dy <= 1; dy += 2)
        {
            line(x + dx * inner, y + dy * inner,
                 x + dx * halfSize, y + dy * inner);
            line(x + dx * inner, y + dy * inner,
                 x + dx * inner, y + dy * halfSize);
        }
    setlinestyle(SOLID_LINE, 0, 1); // 恢复线宽，避免下一帧的棋盘线变粗。
}
