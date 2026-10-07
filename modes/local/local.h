// 文件职责：本地双人模式：点击落子、保存/悔棋/重赛按钮和本次运行内的胜负计数。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#ifndef WUZIQI_LOCAL_H // 头文件防重复包含：只有尚未定义该标记时才展开下面的声明。
#define WUZIQI_LOCAL_H // 定义本头文件的包含标记，防止同一编译单元重复声明。
#include "../../game.h" // 引入 "../../game.h"，提供本文件使用的类型和函数声明。

struct LocalScore // 定义 LocalScore，集中组织相关数据。
{ // 开始上方函数、条件、循环或类型的作用域。
    int blackWins = 0, whiteWins = 0, draws = 0; // 声明 blackWins：累计黑棋胜场；按右侧表达式初始化。
    int countedResult = 0; // 声明 countedResult：本局已计入的结果（0未结算、1黑胜、2白胜、3和棋）；按右侧表达式初始化。
};
bool localFinished(const GameState& game); // 声明接口：判断本地对局是否获胜或满盘。
void updateLocalScore(const GameState& game, LocalScore& score); // 声明接口：先撤销旧结算再登记新结果，重复调用不重复加分，悔棋能撤销结果。
void startLocalGame(GameState& game, LocalScore& score); // 声明接口：清空当前棋盘及本局结算标记，保留累计黑胜、白胜和和棋次数。
bool localRestartClick(int x, int y); // 声明接口：检查再来一局按钮位置；调用者另检查是否已经结束。

// 本地对决处理一次棋盘点击：x、y 是窗口像素坐标。
// 先换算行列，再调用 placePiece；落子成功返回 true，失败返回 false。
// 会修改 game；本地模式允许黑白双方轮流用鼠标下棋。
bool localMove(GameState& game, int x, int y); // 声明接口：将点击转换为行列并调用共用落子规则。

// 判断是否点击本地模式的悔棋按钮。
bool localUndoClick(int x, int y); // 声明接口：检查本地悔棋按钮位置。
bool localSaveClick(int x, int y); // 声明接口：检查本地保存棋谱按钮位置。

// 调用共用 drawMatch 画本地对局，提供本地标题、轮次状态并允许显示准星。
// game 只读；mouseX、mouseY 用于准星和按钮悬停效果。
void drawLocalGame(const GameState& game, const LocalScore& score, int mouseX, int mouseY); // 声明接口：绘制本地棋盘、黑色大字体计数和按状态出现的功能按钮。
#endif // 结束头文件防重复包含的条件范围。
