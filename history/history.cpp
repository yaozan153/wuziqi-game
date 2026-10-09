// 文件职责：棋谱列表与复盘：replay 是当前观看进度，savedGame 是完整存档，续玩使用后者。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#include "history.h" // 引入 "history.h"，提供本文件使用的类型和函数声明。
#include "../ui/menu.h" // 引入 "../ui/menu.h"，提供本文件使用的类型和函数声明。
#include "../ui/match.h" // 引入 "../ui/match.h"，提供本文件使用的类型和函数声明。
#include <ege.h> // 引入 <ege.h>，提供本文件使用的类型和函数声明。
#include <algorithm> // 引入 <algorithm>，提供本文件使用的类型和函数声明。
using namespace ege; // 允许直接使用该命名空间中的名称，省略 ege:: 或 std:: 前缀。

bool HistoryScreen::canResume() const // 函数入口：只允许有棋步、未获胜且未满盘的完整存档继续本地对局。
{ // 开始上方函数、条件、循环或类型的作用域。
    return playing && !savedGame.moves.empty() && savedGame.winner == EMPTY && // 返回 `playing && !savedGame.moves.empty() && savedGame.winner == EMPTY &&`，将结果交给调用者。
           savedGame.moves.size() < BOARD_SIZE * BOARD_SIZE; // 执行 `savedGame.moves.size() < BOARD_SIZE * BOARD_SIZE`；调用相应对象的方法完成本步骤。
}

void HistoryScreen::refresh() // 重置历史页面并扫描棋谱，按保存时间从新到旧排列。
{ // 开始上方函数、条件、循环或类型的作用域。
    files.clear(); moves.clear(); replay = GameState(); // 更新数据：`files.clear(); moves.clear(); replay = GameState()`；赋值后的状态供后续逻辑或绘图使用。
    filename.clear(); message.clear(); page = 0; playing = false; // 更新数据：`filename.clear(); message.clear(); page = 0; playing = false`；赋值后的状态供后续逻辑或绘图使用。
    savedGame = GameState(); resumeRequested = false; // 重新进入历史页面时清空上份完整存档与旧续玩请求。
    WIN32_FIND_DATAW entry; // 声明 entry：扫描到的文件信息；参数或长度由本行给出。
    HANDLE search = FindFirstFileW((recordDirectory() + L"\\*.gmk").c_str(), &entry); // 声明 search：文件搜索句柄；按右侧表达式初始化。
    if (search == INVALID_HANDLE_VALUE) // 检查 `if (search == INVALID_HANDLE_VALUE)`；条件成立时执行括号之后或下一行的处理。
    { // 开始上方函数、条件、循环或类型的作用域。
        const DWORD error = GetLastError(); // 声明 error：错误原因；按右侧表达式初始化。
        if (error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND) // 检查 `if (error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND)`；条件成立时执行括号之后或下一行的处理。
            message = L"无法读取棋谱文件夹"; // 更新数据：`message = L"无法读取棋谱文件夹"`；赋值后的状态供后续逻辑或绘图使用。
        return; // 提前结束当前无返回值函数，避免继续执行后续处理。
    }
    struct RecordEntry { std::wstring name; FILETIME savedAt; }; // 保存文件名及最后写入时间。
    std::vector<RecordEntry> records; // 先保留时间信息，再生成页面的文件名列表。
    do { if (!(entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) records.push_back({entry.cFileName, entry.ftLastWriteTime}); } // 收集棋谱文件，跳过文件夹。
    while (FindNextFileW(search, &entry)); // 只要 `FindNextFileW(search, &entry))` 成立就继续处理；用于事件、连子或网络队列。
    FindClose(search); // 释放文件搜索句柄；参数为 `search)`。
    std::sort(records.begin(), records.end(), [](const RecordEntry& a, const RecordEntry& b) { // 最新保存的棋谱排在前面。
        const LONG order = CompareFileTime(&a.savedAt, &b.savedAt); // 比较文件最后写入时间。
        return order != 0 ? order > 0 : a.name < b.name; // 时间相同时按文件名排序，保持确定的显示顺序。
    }); // 完成时间排序。
    for (const RecordEntry& record : records) files.push_back(record.name); // 填充回放列表。
}

void drawHistory(const HistoryScreen& history, int mouseX, int mouseY) // 函数入口：按 playing 决定画文件列表或复盘页面，续玩按钮依据完整存档状态。
{ // 开始上方函数、条件、循环或类型的作用域。
    if (history.playing) // 检查 `if (history.playing)`；条件成立时执行括号之后或下一行的处理。
    { // 开始上方函数、条件、循环或类型的作用域。
        drawMatch(history.replay, L"棋谱回放", L"手动逐步查看棋局", // 绘制共用棋盘页；canMove 只控制准星，实际合法性由落子函数检查。
                  matchStatus(history.replay), false, mouseX, mouseY); // 根据赢家和满盘状态返回胜负、和棋或轮次文字。
        setfont(18, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `18, 0, L"微软雅黑")`。
        const std::wstring count = L"第 " + std::to_wstring(history.replay.moves.size()) + // 声明 count：连续棋子数或复盘进度文字；按右侧表达式初始化。
            L" / " + std::to_wstring(history.moves.size()) + L" 步"; // 执行 `L" / " + std::to_wstring(history.moves.size()) + L" 步"`；调用相应对象的方法完成本步骤。
        outtextxy(565, 280, count.c_str()); // 在指定坐标绘制文字；参数为 `565, 280, count.c_str())`。
        if (!history.replay.moves.empty()) drawButton(560, 315, 100, 44, L"上一步", mouseX, mouseY); // 检查 `if (!history.replay.moves.empty()) drawButton(560, 315, 100, 44, L"上一步", mouseX, mouseY)`；条件成立时执行括号之后或下一行的处理。
        if (history.replay.moves.size() < history.moves.size()) drawButton(670, 315, 100, 44, L"下一步", mouseX, mouseY); // 检查 `if (history.replay.moves.size() < history.moves.size()) drawButton(670, 315, 100, 44, L"下一步", mouseX, mouseY)`；条件成立时执行括号之后或下一行的处理。
        drawButton(560, 370, 210, 44, L"返回列表", mouseX, mouseY); // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
        if (history.canResume()) // 以完整存档的最终状态决定是否显示续玩按钮，不看复盘当前步数。
            drawButton(560, 490, 210, 44, L"继续本地对局", mouseX, mouseY); // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
        setfont(16, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `16, 0, L"微软雅黑")`。
        outtextxy(28, 550, history.filename.c_str()); // 在指定坐标绘制文字；参数为 `28, 550, history.filename.c_str())`。
        return; // 提前结束当前无返回值函数，避免继续执行后续处理。
    }
    setbkmode(TRANSPARENT); setcolor(EGERGB(45, 37, 26)); setfont(34, 0, L"微软雅黑"); // 设置文字背景透明方式；参数为 `TRANSPARENT); setcolor(EGERGB(45, 37, 26)); setfont(34, 0, L"微软雅黑")`。
    outtextxy(330, 50, L"历史棋谱"); // 在指定坐标绘制文字；参数为 `330, 50, L"历史棋谱")`。
    setfont(18, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `18, 0, L"微软雅黑")`。
    outtextxy(130, 105, L"点击棋谱打开回放；本地对局中可手动保存"); // 在指定坐标绘制文字；参数为 `130, 105, L"点击棋谱打开回放；本地对局中可手动保存")`。
    if (history.files.empty()) outtextxy(250, 240, L"暂无棋谱，请先保存一局本地对局"); // 检查 `if (history.files.empty()) outtextxy(250, 240, L"暂无棋谱，请先保存一局本地对局")`；条件成立时执行括号之后或下一行的处理。
    for (std::size_t i = 0; i < 5 && history.page * 5 + i < history.files.size(); ++i) // 循环推进：`std::size_t i = 0; i < 5 && history.page * 5 + i < history.files.size(); ++i)`；逐项处理棋格、方向、字符或列表元素。
        drawButton(120, 145 + static_cast<int>(i) * 57, 560, 46, // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
                   history.files[history.page * 5 + i].c_str(), mouseX, mouseY); // 执行 `history.files[history.page * 5 + i].c_str(), mouseX, mouseY)`；调用相应对象的方法完成本步骤。
    if (history.page > 0) drawButton(120, 445, 150, 44, L"上一页", mouseX, mouseY); // 检查 `if (history.page > 0) drawButton(120, 445, 150, 44, L"上一页", mouseX, mouseY)`；条件成立时执行括号之后或下一行的处理。
    if ((history.page + 1) * 5 < history.files.size()) drawButton(530, 445, 150, 44, L"下一页", mouseX, mouseY); // 检查 `if ((history.page + 1) * 5 < history.files.size()) drawButton(530, 445, 150, 44, L"下一页", mouseX, mouseY)`；条件成立时执行括号之后或下一行的处理。
    setcolor(EGERGB(45, 37, 26)); setfont(18, 0, L"微软雅黑"); // 设置后续线条和文字颜色；参数为 `EGERGB(45, 37, 26)); setfont(18, 0, L"微软雅黑")`。
    const std::wstring count = L"共 " + std::to_wstring(history.files.size()) + L" 份棋谱"; // 声明 count：连续棋子数或复盘进度文字；按右侧表达式初始化。
    outtextxy(320, 457, count.c_str()); // 在指定坐标绘制文字；参数为 `320, 457, count.c_str())`。
    drawButton(280, 505, 240, 44, L"返回菜单", mouseX, mouseY); // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
    setcolor(EGERGB(45, 37, 26)); setfont(16, 0, L"微软雅黑"); // 设置后续线条和文字颜色；参数为 `EGERGB(45, 37, 26)); setfont(16, 0, L"微软雅黑")`。
    outtextxy(120, 565, history.message.c_str()); // 在指定坐标绘制文字；参数为 `120, 565, history.message.c_str())`。
}

bool historyClick(HistoryScreen& history, int x, int y) // 函数入口：处理列表选择、复盘进退和续玩请求；返回 true 只代表退出到菜单。
{ // 开始上方函数、条件、循环或类型的作用域。
    if (history.playing) // 检查 `if (history.playing)`；条件成立时执行括号之后或下一行的处理。
    { // 开始上方函数、条件、循环或类型的作用域。
        if (matchBack(x, y)) return true; // 检查 `if (matchBack(x, y)) return true`；条件成立时执行括号之后或下一行的处理。
        if (insideButton(x, y, 560, 490, 210, 44) && history.canResume()) // 检查 `if (insideButton(x, y, 560, 490, 210, 44) && history.canResume())`；条件成立时执行括号之后或下一行的处理。
        { // 开始上方函数、条件、循环或类型的作用域。
            history.resumeRequested = true; // 通知 main.cpp 用户请求续玩，本模块自身不切换页面。
            return false; // 当前点击处理结束，继续留在此页面；不是操作失败的标记。
        }
        if (insideButton(x, y, 560, 370, 210, 44)) // 检查 `if (insideButton(x, y, 560, 370, 210, 44))`；条件成立时执行括号之后或下一行的处理。
        { history.playing = false; } // 更新数据：`{ history.playing = false; }`；赋值后的状态供后续逻辑或绘图使用。
        else if (insideButton(x, y, 560, 315, 100, 44)) undoLastMove(history.replay); // 前一条件不满足时，再检查 `(insideButton(x, y, 560, 315, 100, 44)) undoLastMove(history.replay)`。
        else if (insideButton(x, y, 670, 315, 100, 44) && history.replay.moves.size() < history.moves.size()) // 前一条件不满足时，再检查 `(insideButton(x, y, 670, 315, 100, 44) && history.replay.moves.size() < history.moves.size())`。
        { // 开始上方函数、条件、循环或类型的作用域。
            const Move& move = history.moves[history.replay.moves.size()]; // 用当前已播放步数作为下一步下标，取出存档中的下一颗棋子。
            placePiece(history.replay, move.row, move.col); // 只给观看中的复盘棋局加一步，完整存档 savedGame 保持不变。
        }
        return false; // 当前点击处理结束，继续留在此页面；不是操作失败的标记。
    }
    if (insideButton(x, y, 280, 505, 240, 44)) return true; // 检查 `if (insideButton(x, y, 280, 505, 240, 44)) return true`；条件成立时执行括号之后或下一行的处理。
    for (std::size_t i = 0; i < 5 && history.page * 5 + i < history.files.size(); ++i) // 循环推进：`std::size_t i = 0; i < 5 && history.page * 5 + i < history.files.size(); ++i)`；逐项处理棋格、方向、字符或列表元素。
        if (insideButton(x, y, 120, 145 + static_cast<int>(i) * 57, 560, 46)) // 检查 `if (insideButton(x, y, 120, 145 + static_cast<int>(i) * 57, 560, 46))`；条件成立时执行括号之后或下一行的处理。
        { // 开始上方函数、条件、循环或类型的作用域。
            history.filename = history.files[history.page * 5 + i]; // 更新数据：`history.filename = history.files[history.page * 5 + i]`；赋值后的状态供后续逻辑或绘图使用。
            if (loadRecord(history.filename, history.moves, history.message)) // 检查 `if (loadRecord(history.filename, history.moves, history.message))`；条件成立时执行括号之后或下一行的处理。
            { // 开始上方函数、条件、循环或类型的作用域。
                history.savedGame = GameState(); // 更新数据：`history.savedGame = GameState()`；赋值后的状态供后续逻辑或绘图使用。
                for (const Move& move : history.moves) // 循环推进：`const Move& move : history.moves)`；逐项处理棋格、方向、字符或列表元素。
                    placePiece(history.savedGame, move.row, move.col); // 按存档顺序重放每一步，得到保存时的完整棋盘、轮次和胜负。
                history.replay = GameState(); history.playing = true; // 更新数据：`history.replay = GameState(); history.playing = true`；赋值后的状态供后续逻辑或绘图使用。
                history.resumeRequested = false; // 消费或重置续玩请求，避免下一次点击再次切换页面。
            }
            return false; // 当前点击处理结束，继续留在此页面；不是操作失败的标记。
        }
    if (insideButton(x, y, 120, 445, 150, 44) && history.page > 0) --history.page; // 检查 `if (insideButton(x, y, 120, 445, 150, 44) && history.page > 0) --history.page`；条件成立时执行括号之后或下一行的处理。
    else if (insideButton(x, y, 530, 445, 150, 44) && (history.page + 1) * 5 < history.files.size()) ++history.page; // 前一条件不满足时，再检查 `(insideButton(x, y, 530, 445, 150, 44) && (history.page + 1) * 5 < history.files.size()) ++history.page`。
    return false; // 当前点击处理结束，继续留在此页面；不是操作失败的标记。
}
