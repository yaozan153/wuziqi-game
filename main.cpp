#include <ege.h>     
#include "board.h"  
using namespace ege; 
int main()
{
    initgraph(800, 600); 
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

    drawBoard(); 
    GameState game; 

    while (is_run()) 
    {
        
        if (kbhit() && getch() == 27) 
            break; 

        while (mousemsg()) // 只要鼠标消息队列不为空，就继续处理下一条消息。
        {
            mouse_msg msg = getmouse(); 
            if (msg.is_left() && msg.is_down()) // 仅在左键按下时落子，移动或松开不落子。
            {
                // placePiece 返回 true 表示落子成功；失败时不切换玩家。
                placePiece(game, msg.x, msg.y);
                    
            }
        }

        delay_fps(60);
    }

    freePieceImages(); // 先释放图片，再关闭图形窗口。
    closegraph(); 
    return 0; 
}
