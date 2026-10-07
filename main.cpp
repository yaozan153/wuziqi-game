// 文件职责：程序入口：持有各模式状态，处理键盘和鼠标、切换页面、每帧绘图及显示胜利弹窗。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#include <ege.h>  
#include "game.h" 
#include "board.h" 
#include "ui/menu.h" 
#include "ui/match.h" 
#include "modes/local/local.h" 
#include "modes/human_ai/human_ai.h" 
#include "modes/online/online.h" 
#include "history/history.h" 
using namespace ege; 
int main() 
{ 
    initgraph(800, 600, INIT_RENDERMANUAL);
    setcaption(L"五子棋游戏"); 

    setbkcolor(EGERGB(244, 199, 122)); 
    cleardevice(); 

    if (!loadPieceImages()) 
    { // 开始上方函数、条件、循环或类型的作用域。
        MessageBoxW(NULL, L"无法加载棋子或木纹图片，请检查 assets 文件夹。", L"五子棋", MB_OK); // 显示 Windows Unicode 提示框；参数为 `NULL, L"无法加载棋子或木纹图片，请检查 assets 文件夹。", L"五子棋", MB_OK)`。
        freePieceImages(); // 释放三张素材图片并将指针清空，防止再次使用已释放对象。
        closegraph(); // 关闭图形窗口；参数为 `)`。
        return 1; // 初始化失败，向操作系统返回非零退出码。
    }

    GameState game; // 声明 game：当前棋局；参数或长度由本行给出。
    LocalScore localScore; // 声明 localScore：本次程序运行的本地累计战绩；参数或长度由本行给出。
    OnlineSession online; // 声明 online：联机会话；参数或长度由本行给出。
    HistoryScreen history; // 声明 history：历史棋谱页面状态；参数或长度由本行给出。
    bool winMessageShown = false; // 获胜提示只显示一次。
    Screen screen = Screen::MENU; // 主循环只负责页面切换，模式代码放在各自文件夹。
    PieceColor humanPiece = EMPTY; // 每次进入人机模式都先让玩家选择颜色。
    // 在离屏图片里画完整一帧，再一次性显示，避免擦准星时破坏木纹或闪烁。
    PIMAGE frame = newimage(800, 600); // 声明 frame：整帧离屏绘图缓冲；按右侧表达式初始化。
    setbkcolor(EGERGB(244, 199, 122), frame); // 设置绘图背景色；参数为 `EGERGB(244, 199, 122), frame)`。

    while (is_run() && screen != Screen::EXIT) // 只要 `is_run() && screen != Screen::EXIT)` 成立就继续处理；用于事件、连子或网络队列。
    { // 开始上方函数、条件、循环或类型的作用域。

        while (kbhit()) // 只要 `kbhit())` 成立就继续处理；用于事件、连子或网络队列。
        { // 开始上方函数、条件、循环或类型的作用域。
            int key = getch(); // 声明 key：本次键盘键值；按右侧表达式初始化。
            if (key == 0 || key == 224) // 检查 `if (key == 0 || key == 224)`；条件成立时执行括号之后或下一行的处理。
                key = 256 + getch(); // 更新数据：`key = 256 + getch()`；赋值后的状态供后续逻辑或绘图使用。
            if (key == 27) // 检查 `if (key == 27)`；条件成立时执行括号之后或下一行的处理。
            { // 开始上方函数、条件、循环或类型的作用域。
                if (screen == Screen::MENU) // 检查 `if (screen == Screen::MENU)`；条件成立时执行括号之后或下一行的处理。
                { // 开始上方函数、条件、循环或类型的作用域。
                    screen = Screen::EXIT; // 更新数据：`screen = Screen::EXIT`；赋值后的状态供后续逻辑或绘图使用。
                    break; // 跳出当前循环，继续执行循环之后的代码。
                }
                if (screen == Screen::ONLINE) // 检查 `if (screen == Screen::ONLINE)`；条件成立时执行括号之后或下一行的处理。
                    online.reset(); // 执行 `online.reset()`；调用相应对象的方法完成本步骤。
                screen = Screen::MENU; // 返回主菜单；不会在这里自动写入棋谱文件。
                flushmouse(); // 清掉旧鼠标事件，避免跨页面误落子；参数为 `)`。
                break; // 跳出当前循环，继续执行循环之后的代码。
            }
            if (screen == Screen::ONLINE) // 检查 `if (screen == Screen::ONLINE)`；条件成立时执行括号之后或下一行的处理。
                onlineKey(online, key); // 编辑 IPv4 输入框光标及数字点号，Enter 发起加入。
        }
        if (screen == Screen::EXIT) // 检查 `if (screen == Screen::EXIT)`；条件成立时执行括号之后或下一行的处理。
            break; // 跳出当前循环，继续执行循环之后的代码。

        settarget(frame); // 切换 EGE 绘图目标；参数为 `frame)`。
        while (mousemsg()) // 只要鼠标消息队列不为空，就继续处理下一条消息。
        { // 开始上方函数、条件、循环或类型的作用域。
            mouse_msg msg = getmouse(); // 更新数据：`mouse_msg msg = getmouse()`；赋值后的状态供后续逻辑或绘图使用。
            if (msg.is_left() && msg.is_down()) // 仅在左键按下时落子，移动或松开不落子。
            { // 开始上方函数、条件、循环或类型的作用域。
                if (screen == Screen::MENU) // 检查 `if (screen == Screen::MENU)`；条件成立时执行括号之后或下一行的处理。
                { // 开始上方函数、条件、循环或类型的作用域。
                    screen = menuChoice(msg.x, msg.y); // 更新数据：`screen = menuChoice(msg.x, msg.y)`；赋值后的状态供后续逻辑或绘图使用。
                    if (screen == Screen::LOCAL) // 检查 `if (screen == Screen::LOCAL)`；条件成立时执行括号之后或下一行的处理。
                    { // 开始上方函数、条件、循环或类型的作用域。
                        startLocalGame(game, localScore); // 清空当前棋盘及本局结算标记，保留累计黑胜、白胜和和棋次数。
                        winMessageShown = false; // 清除本局胜利弹窗标记，新局或悔棋后再次获胜可重新提示。
                    }
                    if (screen == Screen::HUMAN_AI_SELECT) // 检查 `if (screen == Screen::HUMAN_AI_SELECT)`；条件成立时执行括号之后或下一行的处理。
                        humanPiece = EMPTY; // 更新数据：`humanPiece = EMPTY`；赋值后的状态供后续逻辑或绘图使用。
                    if (screen == Screen::HISTORY) // 检查 `if (screen == Screen::HISTORY)`；条件成立时执行括号之后或下一行的处理。
                        history.refresh(); // 执行 `history.refresh()`；调用相应对象的方法完成本步骤。
                    if (screen == Screen::ONLINE) // 检查 `if (screen == Screen::ONLINE)`；条件成立时执行括号之后或下一行的处理。
                    { // 开始上方函数、条件、循环或类型的作用域。
                        online.reset(); // 执行 `online.reset()`；调用相应对象的方法完成本步骤。
                        winMessageShown = false; // 清除本局胜利弹窗标记，新局或悔棋后再次获胜可重新提示。
                    }
                    if (screen != Screen::MENU) // 检查 `if (screen != Screen::MENU)`；条件成立时执行括号之后或下一行的处理。
                        flushmouse(); // 页面切换时不把旧点击传给新页面。
                }
                else if (screen == Screen::HUMAN_AI_SELECT) // 前一条件不满足时，再检查 `(screen == Screen::HUMAN_AI_SELECT)`。
                { // 开始上方函数、条件、循环或类型的作用域。
                    if (humanAiChoiceBack(msg.x, msg.y)) // 检查 `if (humanAiChoiceBack(msg.x, msg.y))`；条件成立时执行括号之后或下一行的处理。
                        screen = Screen::MENU; // 返回主菜单；不会在这里自动写入棋谱文件。
                    else // 前面的条件不满足时，执行下面的备用分支。
                    { // 开始上方函数、条件、循环或类型的作用域。
                        humanPiece = humanAiChoice(msg.x, msg.y); // 更新数据：`humanPiece = humanAiChoice(msg.x, msg.y)`；赋值后的状态供后续逻辑或绘图使用。
                        if (humanPiece != EMPTY) // 检查 `if (humanPiece != EMPTY)`；条件成立时执行括号之后或下一行的处理。
                        { // 开始上方函数、条件、循环或类型的作用域。
                            game = GameState(); // 始终黑棋先下，选白不改变 currentPlayer。
                            winMessageShown = false; // 清除本局胜利弹窗标记，新局或悔棋后再次获胜可重新提示。
                            screen = Screen::HUMAN_AI; // 结束选色并进入人机棋盘页面，电脑回合由每帧逻辑处理。
                        }
                    }
                    if (screen != Screen::HUMAN_AI_SELECT) // 检查 `if (screen != Screen::HUMAN_AI_SELECT)`；条件成立时执行括号之后或下一行的处理。
                        flushmouse(); // 清掉旧鼠标事件，避免跨页面误落子；参数为 `)`。
                }
                else if (screen == Screen::LOCAL || screen == Screen::HUMAN_AI) // 前一条件不满足时，再检查 `(screen == Screen::LOCAL || screen == Screen::HUMAN_AI)`。
                { // 开始上方函数、条件、循环或类型的作用域。
                    if (matchBack(msg.x, msg.y)) // 检查 `if (matchBack(msg.x, msg.y))`；条件成立时执行括号之后或下一行的处理。
                        screen = Screen::MENU; // 返回主菜单；不会在这里自动写入棋谱文件。
                    else if (screen == Screen::LOCAL) // 前一条件不满足时，再检查 `(screen == Screen::LOCAL)`。
                    { // 开始上方函数、条件、循环或类型的作用域。
                        if (localRestartClick(msg.x, msg.y) && localFinished(game)) // 检查 `if (localRestartClick(msg.x, msg.y) && localFinished(game))`；条件成立时执行括号之后或下一行的处理。
                        { // 开始上方函数、条件、循环或类型的作用域。
                            startLocalGame(game, localScore); // 清空当前棋盘及本局结算标记，保留累计黑胜、白胜和和棋次数。
                            winMessageShown = false; // 清除本局胜利弹窗标记，新局或悔棋后再次获胜可重新提示。
                            flushmouse(); // 清掉旧鼠标事件，避免跨页面误落子；参数为 `)`。
                        }
                        else if (localUndoClick(msg.x, msg.y)) // 前一条件不满足时，再检查 `(localUndoClick(msg.x, msg.y))`。
                        { // 开始上方函数、条件、循环或类型的作用域。
                            if (undoLastMove(game)) // 检查 `if (undoLastMove(game))`；条件成立时执行括号之后或下一行的处理。
                                winMessageShown = false; // 清除本局胜利弹窗标记，新局或悔棋后再次获胜可重新提示。
                        }
                        else if (localSaveClick(msg.x, msg.y) && !game.moves.empty()) // 前一条件不满足时，再检查 `(localSaveClick(msg.x, msg.y) && !game.moves.empty())`。
                        { // 开始上方函数、条件、循环或类型的作用域。
                            wchar_t name[128] = {}; // 声明 name：用户输入的棋谱名称；按右侧表达式初始化。
                            if (inputbox_getline(L"保存棋谱", L"请输入棋谱名称（可留空使用时间，最多 80 字）：", name, 128)) // 检查 `if (inputbox_getline(L"保存棋谱", L"请输入棋谱名称（可留空使用时间，最多 80 字）：", name, 128))`；条件成立时执行括号之后或下一行的处理。
                            { // 开始上方函数、条件、循环或类型的作用域。
                                std::wstring filename, error; // 声明 filename：棋谱文件名；参数或长度由本行给出。
                                const bool saved = saveRecord(game, filename, error, name); // 用户确认名称后才尝试写入棋谱；返回值表示文件是否完整写入成功。
                                const std::wstring message = saved ? L"已保存到 records 文件夹：\n" + filename : error; // 声明 message：待显示的提示文字；按右侧表达式初始化。
                                MessageBoxW(getHWnd(), message.c_str(), L"保存棋谱", MB_OK); // 显示 Windows Unicode 提示框；参数为 `getHWnd(), message.c_str(), L"保存棋谱", MB_OK)`。
                            }
                            flushmouse(); // 清掉旧鼠标事件，避免跨页面误落子；参数为 `)`。
                            while (kbhit()) getch(); // 关闭命名弹窗后清空残留按键，避免 Enter 或 Esc 再影响游戏页面。
                        }
                        else // 前面的条件不满足时，执行下面的备用分支。
                            localMove(game, msg.x, msg.y); // 将点击转换为行列并调用共用落子规则。
                        updateLocalScore(game, localScore); // 先撤销旧结算再登记新结果，重复调用不重复加分，悔棋能撤销结果。
                    }
                    else // 前面的条件不满足时，执行下面的备用分支。
                        humanAiMove(game, humanPiece, msg.x, msg.y); // 仅允许玩家回合处理点击并通过共用规则落子。
                }
                else if (screen == Screen::ONLINE) // 前一条件不满足时，再检查 `(screen == Screen::ONLINE)`。
                { // 开始上方函数、条件、循环或类型的作用域。
                    if (onlineClick(online, msg.x, msg.y)) // 检查 `if (onlineClick(online, msg.x, msg.y))`；条件成立时执行括号之后或下一行的处理。
                    { // 开始上方函数、条件、循环或类型的作用域。
                        screen = Screen::MENU; // 返回主菜单；不会在这里自动写入棋谱文件。
                        flushmouse(); // 清掉旧鼠标事件，避免跨页面误落子；参数为 `)`。
                    }
                }
                else if (screen == Screen::HISTORY) // 前一条件不满足时，再检查 `(screen == Screen::HISTORY)`。
                { // 开始上方函数、条件、循环或类型的作用域。
                    const bool wasPlaying = history.playing; // 声明 wasPlaying：处理点击前是否在回放页；按右侧表达式初始化。
                    if (historyClick(history, msg.x, msg.y)) // 检查 `if (historyClick(history, msg.x, msg.y))`；条件成立时执行括号之后或下一行的处理。
                        screen = Screen::MENU; // 返回主菜单；不会在这里自动写入棋谱文件。
                    else if (history.resumeRequested) // 前一条件不满足时，再检查 `(history.resumeRequested)`。
                    { // 开始上方函数、条件、循环或类型的作用域。
                        game = history.savedGame; // 续玩复制完整存档棋局，不采用当前复盘停留的那一步。
                        localScore.countedResult = 0; // 载入的是未结束棋局，本局尚未计分；保留此前其他对局的累计战绩。
                        history.resumeRequested = false; // 消费或重置续玩请求，避免下一次点击再次切换页面。
                        winMessageShown = false; // 清除本局胜利弹窗标记，新局或悔棋后再次获胜可重新提示。
                        screen = Screen::LOCAL; // 切换到本地双人对局，后续点击会进入本地落子处理分支。
                    }
                    if (screen != Screen::HISTORY || wasPlaying != history.playing) // 检查 `if (screen != Screen::HISTORY || wasPlaying != history.playing)`；条件成立时执行括号之后或下一行的处理。
                        flushmouse(); // 清掉旧鼠标事件，避免跨页面误落子；参数为 `)`。
                }
            }
        }

        if (screen == Screen::EXIT) // 检查 `if (screen == Screen::EXIT)`；条件成立时执行括号之后或下一行的处理。
            break; // 跳出当前循环，继续执行循环之后的代码。
        // 消息处理完后电脑只走一步；选白时也会自动完成黑棋首手。
        if (screen == Screen::HUMAN_AI && computerAiMove(game, humanPiece)) // 检查 `if (screen == Screen::HUMAN_AI && computerAiMove(game, humanPiece))`；条件成立时执行括号之后或下一行的处理。
            flushmouse(); // 不把电脑计算期间的点击带到玩家的下一回合。
        if (screen == Screen::ONLINE) // 检查 `if (screen == Screen::ONLINE)`；条件成立时执行括号之后或下一行的处理。
            online.update(); // 执行 `online.update()`；调用相应对象的方法完成本步骤。
        // 每帧获取实时鼠标位置；不在窗口上时不显示按钮悬停或准星。
        int mouseX = -1000, mouseY = -1000; // 声明 mouseX：鼠标横坐标；按右侧表达式初始化。
        POINT cursor; // 声明 cursor：系统鼠标位置；参数或长度由本行给出。
        if (GetCursorPos(&cursor) && WindowFromPoint(cursor) == getHWnd()) // 检查 `if (GetCursorPos(&cursor) && WindowFromPoint(cursor) == getHWnd())`；条件成立时执行括号之后或下一行的处理。
        { // 开始上方函数、条件、循环或类型的作用域。
            ScreenToClient(getHWnd(), &cursor); // 将屏幕坐标转为窗口客户区坐标；参数为 `getHWnd(), &cursor)`。
            mouseX = cursor.x; // 更新数据：`mouseX = cursor.x`；赋值后的状态供后续逻辑或绘图使用。
            mouseY = cursor.y; // 更新数据：`mouseY = cursor.y`；赋值后的状态供后续逻辑或绘图使用。
        }

        const bool inMatch = screen == Screen::LOCAL || screen == Screen::HUMAN_AI || // 声明 inMatch：当前是否是可显示胜利提示的对局页面；按右侧表达式初始化。
                             (screen == Screen::ONLINE && online.localPiece != EMPTY); // 执行 `(screen == Screen::ONLINE && online.localPiece != EMPTY)`；调用相应对象的方法完成本步骤。
        setbkcolor(inMatch || (screen == Screen::HISTORY && history.playing) // 设置绘图背景色；参数为 `inMatch || (screen == Screen::HISTORY && history.playing)`。
                   ? EGERGB(244, 199, 122) : EGERGB(37, 40, 43)); // 条件表达式：`? EGERGB(244, 199, 122) : EGERGB(37, 40, 43))`；根据条件选择两个值之一。
        cleardevice(); // 清空当前绘图目标；参数为 `)`。
        if (screen == Screen::MENU) // 检查 `if (screen == Screen::MENU)`；条件成立时执行括号之后或下一行的处理。
            drawMenu(mouseX, mouseY); // 绘制首页标题和五个入口按钮。
        else if (screen == Screen::LOCAL) // 前一条件不满足时，再检查 `(screen == Screen::LOCAL)`。
            drawLocalGame(game, localScore, mouseX, mouseY); // 绘制本地棋盘、黑色大字体计数和按状态出现的功能按钮。
        else if (screen == Screen::HUMAN_AI) // 前一条件不满足时，再检查 `(screen == Screen::HUMAN_AI)`。
            drawHumanAiScreen(game, humanPiece, mouseX, mouseY); // 按玩家颜色与当前回合绘制人机状态，满盘无赢家显示和棋。
        else if (screen == Screen::HUMAN_AI_SELECT) // 前一条件不满足时，再检查 `(screen == Screen::HUMAN_AI_SELECT)`。
            drawHumanAiChoice(mouseX, mouseY); // 绘制玩家选黑、选白及返回菜单按钮。
        else if (screen == Screen::ONLINE) // 前一条件不满足时，再检查 `(screen == Screen::ONLINE)`。
            drawOnlineScreen(online, mouseX, mouseY); // 根据大厅、等待房间或已选色对局状态绘制对应联机界面。
        else if (screen == Screen::HISTORY) // 前一条件不满足时，再检查 `(screen == Screen::HISTORY)`。
            drawHistory(history, mouseX, mouseY); // 按 playing 决定画文件列表或复盘页面，续玩按钮依据完整存档状态。

        settarget(NULL); // 切回窗口，将离屏画面整体显示。
        putimage(0, 0, frame); // 绘制完整图片；参数为 `0, 0, frame)`。
        delay_fps(60); // 按每秒60帧节奏推进绘图循环；参数为 `60)`。

        // 先刷新包含最后一颗棋子的画面，再弹出胜利提示。
        const GameState& activeGame = screen == Screen::ONLINE ? online.game : game; // 只读引用当前模式的棋局，联机用online.game，本地/人机用game，避免检查错赢家。
        if (is_run() && inMatch && activeGame.winner != EMPTY && !winMessageShown) // 检查 `if (is_run() && inMatch && activeGame.winner != EMPTY && !winMessageShown)`；条件成立时执行括号之后或下一行的处理。
        { // 开始上方函数、条件、循环或类型的作用域。
            winMessageShown = true; // 记录已显示本局胜利提示，避免主循环下一帧再次弹窗。
            const wchar_t* message = activeGame.winner == BLACK_PIECE // 声明 message：待显示的提示文字；按右侧表达式初始化。
                                     ? L"黑棋获胜！" : L"白棋获胜！"; // 条件表达式：`? L"黑棋获胜！" : L"白棋获胜！"`；根据条件选择两个值之一。
            MessageBoxW(getHWnd(), message, L"游戏结束", MB_OK); // 显示 Windows Unicode 提示框；参数为 `getHWnd(), message, L"游戏结束", MB_OK)`。
            flushmouse(); // 清除弹窗期间积累的鼠标消息。
        }
    }

    online.reset(); // 执行 `online.reset()`；调用相应对象的方法完成本步骤。
    settarget(NULL); // 切换 EGE 绘图目标；参数为 `NULL)`。
    delimage(frame); // 释放用于每帧重画的离屏图片。
    freePieceImages(); // 先释放图片，再关闭图形窗口。
    closegraph(); // 关闭图形窗口；参数为 `)`。
    return 0; // 正常结束主函数，向操作系统返回成功退出码。
}
