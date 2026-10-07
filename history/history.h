// 文件职责：棋谱列表与复盘：replay 是当前观看进度，savedGame 是完整存档，续玩使用后者。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#ifndef WUZIQI_HISTORY_H // 头文件防重复包含：只有尚未定义该标记时才展开下面的声明。
#define WUZIQI_HISTORY_H // 定义本头文件的包含标记，防止同一编译单元重复声明。
#include "record.h" // 引入 "record.h"，提供本文件使用的类型和函数声明。

struct HistoryScreen // 定义 HistoryScreen，集中组织相关数据。
{ // 开始上方函数、条件、循环或类型的作用域。
    std::vector<std::wstring> files; // 声明 files：扫描到的棋谱文件名；参数或长度由本行给出。
    std::vector<Move> moves; // 声明 moves：按先后顺序的棋步记录；参数或长度由本行给出。
    GameState replay; // 声明 replay：正在观看的复盘棋局；参数或长度由本行给出。
    GameState savedGame; // 声明 savedGame：保存时的完整棋局；参数或长度由本行给出。
    std::wstring filename, message; // 声明 filename：棋谱文件名；参数或长度由本行给出。
    std::size_t page = 0; // 声明 page：列表页号，从零开始；按右侧表达式初始化。
    bool playing = false; // 声明 playing：是否处于棋谱回放页；按右侧表达式初始化。
    bool resumeRequested = false; // 声明 resumeRequested：是否请求切换到本地续玩；按右侧表达式初始化。
    bool canResume() const; // 声明接口：只允许有棋步、未获胜且未满盘的完整存档继续本地对局。
    void refresh(); // 声明接口：重置历史页面并扫描 .gmk 文件，以文件名倒序排列。
};
void drawHistory(const HistoryScreen& history, int mouseX, int mouseY); // 声明接口：按 playing 决定画文件列表或复盘页面，续玩按钮依据完整存档状态。

bool historyClick(HistoryScreen& history, int x, int y); // 声明接口：处理列表选择、复盘进退和续玩请求；返回 true 只代表退出到菜单。
#endif // 结束头文件防重复包含的条件范围。
