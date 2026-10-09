

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
    const int PIECE_IMAGE_SIZE = 36; 
    const int PIECE_IMAGE_HALF = PIECE_IMAGE_SIZE / 2; 
    const int boardPadding = CELL_SIZE / 2; 

   
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

bool mouseToBoard(int mouseX, int mouseY, int& row, int& col) {
    const int halfCell = CELL_SIZE / 2; 
    const int length = (BOARD_SIZE - 1) * CELL_SIZE; 

    if (mouseX < BOARD_LEFT - halfCell || mouseX >= BOARD_LEFT + length + halfCell || 
        mouseY < BOARD_TOP - halfCell || mouseY >= BOARD_TOP + length + halfCell) 
        return false; 

    col = (mouseX - BOARD_LEFT + halfCell) / CELL_SIZE; 
    row = (mouseY - BOARD_TOP + halfCell) / CELL_SIZE; 
    return true; 
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

    
    putimage(BOARD_LEFT - boardPadding, BOARD_TOP - boardPadding, boardImage); 

    setcolor(EGERGB(30, 30, 30)); 

    for (int i = 0; i < BOARD_SIZE; i++) 
    { 
        const int x = BOARD_LEFT + i * CELL_SIZE; 
        const int y = BOARD_TOP + i * CELL_SIZE; 

        line(BOARD_LEFT, y, BOARD_LEFT + boardLength, y); 
        line(x, BOARD_TOP, x, BOARD_TOP + boardLength); 
    } 


    const int starPoints[5][2] = {{4, 4}, {4, 10}, {10, 4}, {10, 10}, {7, 7}}; 
    setfillcolor(EGERGB(30, 30, 30)); 
    for (int i = 0; i < 5; i++) 
    { 
        const int x = BOARD_LEFT + (starPoints[i][0] - 1) * CELL_SIZE; 
        const int y = BOARD_TOP + (starPoints[i][1] - 1) * CELL_SIZE; 
        fillellipse(x, y, 3, 3); 
    } 
} 

void drawPiece(int row, int col, PieceColor piece) 
{ 
    
    const int x = BOARD_LEFT + col * CELL_SIZE; 
    const int y = BOARD_TOP + row * CELL_SIZE; 


    PIMAGE image = piece == BLACK_PIECE ? blackImage : whiteImage; 
    putimage_withalpha(NULL, image, x - PIECE_IMAGE_HALF, y - PIECE_IMAGE_HALF); 
} 

void drawLastMoveMarker(int row, int col) // 绘制最后一步标记，供所有对局与复盘共用。
{
    const int x = BOARD_LEFT + col * CELL_SIZE; // 将棋子列号转换为中心横坐标。
    const int y = BOARD_TOP + row * CELL_SIZE; // 将棋子行号转换为中心纵坐标。
    const int radius = 6; // 红圈半径为 6 像素，保持在棋子内部。
    const color_t previousColor = getcolor(); // 保存原来的画笔颜色，避免影响后续绘图。
    setcolor(EGERGB(255, 0, 0)); // 使用红色，黑棋和白棋上都能看到标记。
    setlinestyle(SOLID_LINE, 0, 2); // 使用 2 像素宽的实线绘制红圈。
    circle(x, y, radius); // 绘制空心圆，保留圆内的棋子纹理。
    setlinestyle(SOLID_LINE, 0, 1); // 恢复项目默认的 1 像素实线。
    setcolor(previousColor); // 恢复原来的画笔颜色。
}

void drawCrosshair(int row, int col)
{
    const int x = BOARD_LEFT + col * CELL_SIZE;
    const int y = BOARD_TOP + row * CELL_SIZE;
    const int halfSize = 22;
    const int armLength = 14;
    const int inner = halfSize - armLength;
    setcolor(EGERGB(0, 150, 255));
    setlinestyle(SOLID_LINE, 0, 3);
    for (int dx = -1; dx <= 1; dx += 2)
        for (int dy = -1; dy <= 1; dy += 2)
        {
            line(x + dx * inner, y + dy * inner,
                 x + dx * halfSize, y + dy * inner);
            line(x + dx * inner, y + dy * inner,
                 x + dx * inner, y + dy * halfSize);
        }
    setlinestyle(SOLID_LINE, 0, 1);
}
