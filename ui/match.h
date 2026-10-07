// 文件职责：共用对局画面：绘制棋盘、全部棋步、状态文字与返回按钮。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#ifndef WUZIQI_MATCH_H // 头文件防重复包含：只有尚未定义该标记时才展开下面的声明。
#define WUZIQI_MATCH_H // 定义本头文件的包含标记，防止同一编译单元重复声明。
#include "../game.h" // 引入 "../game.h"，提供本文件使用的类型和函数声明。

// 所有对战模式共用棋盘、状态栏和返回按钮；canMove 决定是否显示落子准星。
// game 只读；title 为模式标题，description 为说明，status 为当前状态文字。
// mouseX、mouseY 是窗口鼠标坐标；canMove 只控制准星，不能阻止实际落子。
// 本函数只绘图，不修改棋局，不切换页面，也不弹出胜利提示。
void drawMatch(const GameState& game, const wchar_t* title, const wchar_t* description, // 函数入口：绘制共用棋盘页；canMove 只控制准星，实际合法性由落子函数检查。
               const wchar_t* status, bool canMove, int mouseX, int mouseY); // 声明 status：当前状态说明；参数或长度由本行给出。

// 判断窗口坐标 x、y 是否点中共用棋盘页的返回按钮，点中返回 true。
bool matchBack(int x, int y); // 声明接口：检查共用棋盘页的返回菜单按钮。

// 根据棋局返回状态文字：轮到黑棋、轮到白棋、黑棋获胜或白棋获胜。
// 返回只读字符串指针，不修改 game，也不负责把文字画出来。
const wchar_t* matchStatus(const GameState& game); // 声明接口：根据赢家和满盘状态返回胜负、和棋或轮次文字。
#endif // 结束头文件防重复包含的条件范围。
