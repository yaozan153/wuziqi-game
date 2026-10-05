#include <ege.h>     
#include "board.h"  
using namespace ege; 
int main()
{
    initgraph(800, 600, INIT_RENDERMANUAL); 
    setcaption(L"五子棋"); 

    setbkcolor(EGERGB(244, 199, 122)); // 棋盘外的区域使用木纹图片的平均颜色。
    cleardevice(); 

    // 图片只加载一次，落子时直接使用；缺少素材时给出提示。
    if (!loadPieceImages())
    {
        MessageBoxW(NULL, L"无法加载棋子或木纹图片，请检查 assets 文件夹。", L"五子棋", MB_OK);
        freePieceImages();
        closegraph();
        return 1;
    }

    GameState game; 
    // 在离屏图片里画完整一帧，再一次性显示，避免擦准星时破坏木纹或闪烁。
    PIMAGE frame = newimage(800, 600);
    setbkcolor(EGERGB(244, 199, 122), frame);

    while (is_run()) 
    {
        
        if (kbhit() && getch() == 27) 
            break; 

        settarget(frame);
        while (mousemsg()) // 只要鼠标消息队列不为空，就继续处理下一条消息。
        {
            mouse_msg msg = getmouse(); 
            if (msg.is_left() && msg.is_down()) // 仅在左键按下时落子，移动或松开不落子。
            {
                // placePiece 返回 true 表示落子成功；失败时不切换玩家。
                placePiece(game, msg.x, msg.y);
                    
            }
        }

        cleardevice();
        drawBoard();
        for (const Move& move : game.moves)
            drawPiece(move.row, move.col, move.piece);

        // 每帧获取实时鼠标位置；离开窗口或位于已占用交点时隐藏准星。
        POINT cursor;
        if (GetCursorPos(&cursor) && WindowFromPoint(cursor) == getHWnd())
        {
            ScreenToClient(getHWnd(), &cursor);
            int row, col;
            if (mouseToBoard(cursor.x, cursor.y, row, col) && game.board[row][col] == EMPTY)
                drawCrosshair(row, col);
        }

        settarget(NULL); // 切回窗口，将离屏画面整体显示。
        putimage(0, 0, frame);
        delay_fps(60);
    }

    settarget(NULL);
    delimage(frame); // 释放用于每帧重画的离屏图片。
    freePieceImages(); // 先释放图片，再关闭图形窗口。
    closegraph(); 
    return 0; 
}
