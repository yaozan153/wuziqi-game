// 文件职责：自动化验证：断言规则、回合、文件或网络行为符合预期；不参与游戏主程序。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#include "../history/history.h" // 引入 "../history/history.h"，提供本文件使用的类型和函数声明。
#include <windows.h> // 引入 <windows.h>，提供本文件使用的类型和函数声明。
#include <cassert> // 引入 <cassert>，提供本文件使用的类型和函数声明。
#include <iostream> // 引入 <iostream>，提供本文件使用的类型和函数声明。

void replaceContents(const std::wstring& name, const std::string& data) // 函数入口：仅测试使用：改写临时棋谱为非法内容，检查读取是否能拒绝。
{ // 开始上方函数、条件、循环或类型的作用域。
    HANDLE file = CreateFileW((recordDirectory() + L"\\" + name).c_str(), GENERIC_WRITE, // 声明 file：打开文件的 Windows 句柄；按右侧表达式初始化。
                             0, NULL, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); // 承接上方表达式的参数、条件或初值：`0, NULL, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)`。
    assert(file != INVALID_HANDLE_VALUE); // 测试断言：要求 `file != INVALID_HANDLE_VALUE` 成立；失败立即终止测试。
    DWORD written = 0; // 声明 written：实际写入字节数；按右侧表达式初始化。
    assert(WriteFile(file, data.data(), static_cast<DWORD>(data.size()), &written, NULL)); // 测试断言：要求 `WriteFile(file, data.data(), static_cast<DWORD>(data.size()), &written, NULL` 成立；失败立即终止测试。
    assert(written == data.size()); // 测试断言：要求 `written == data.size(` 成立；失败立即终止测试。
    CloseHandle(file); // 释放文件或搜索句柄；参数为 `file)`。
}

int main() // 独立测试入口：验证命名、保存、读取、复盘、续玩及非法棋谱；不链接进游戏主程序。
{ // 开始上方函数、条件、循环或类型的作用域。
    GameState game; // 声明 game：当前棋局；参数或长度由本行给出。
    std::wstring filename, second, error; // 声明 filename：棋谱文件名；参数或长度由本行给出。
    assert(!saveRecord(game, filename, error)); // 测试断言：要求 `!saveRecord(game, filename, error` 成立；失败立即终止测试。
    for (int col = 0; col < 5; ++col) // 循环推进：`int col = 0; col < 5; ++col)`；逐项处理棋格、方向、字符或列表元素。
    { // 开始上方函数、条件、循环或类型的作用域。
        assert(placePiece(game, 0, col)); // 测试断言：要求 `placePiece(game, 0, col` 成立；失败立即终止测试。
        if (col < 4) assert(placePiece(game, 2, col)); // 检查 `if (col < 4) assert(placePiece(game, 2, col))`；条件成立时执行括号之后或下一行的处理。
    }
    assert(saveRecord(game, filename, error)); // 测试断言：要求 `saveRecord(game, filename, error` 成立；失败立即终止测试。
    std::vector<Move> loaded; // 声明 loaded：读取出的棋步列表；参数或长度由本行给出。
    assert(loadRecord(filename, loaded, error) && loaded.size() == 9); // 测试断言：要求 `loadRecord(filename, loaded, error) && loaded.size() == 9` 成立；失败立即终止测试。
    GameState replay; // 声明 replay：正在观看的复盘棋局；参数或长度由本行给出。
    for (const Move& move : loaded) assert(placePiece(replay, move.row, move.col)); // 循环推进：`const Move& move : loaded) assert(placePiece(replay, move.row, move.col))`；逐项处理棋格、方向、字符或列表元素。
    assert(replay.winner == BLACK_PIECE); // 测试断言：要求 `replay.winner == BLACK_PIECE` 成立；失败立即终止测试。
    assert(undoLastMove(replay)); // 测试断言：要求 `undoLastMove(replay` 成立；失败立即终止测试。
    assert(replay.winner == EMPTY && placePiece(replay, 0, 4)); // 测试断言：要求 `replay.winner == EMPTY && placePiece(replay, 0, 4` 成立；失败立即终止测试。

    assert(undoLastMove(game)); // 测试断言：要求 `undoLastMove(game` 成立；失败立即终止测试。
    assert(saveRecord(game, second, error) && second != filename); // 测试断言：要求 `saveRecord(game, second, error) && second != filename` 成立；失败立即终止测试。
    assert(loadRecord(second, loaded, error) && loaded.size() == 8); // 测试断言：要求 `loadRecord(second, loaded, error) && loaded.size() == 8` 成立；失败立即终止测试。
    HistoryScreen history; // 声明 history：历史棋谱页面状态；参数或长度由本行给出。
    history.refresh(); // 执行 `history.refresh()`；调用相应对象的方法完成本步骤。
    assert(history.files.size() >= 2 && !history.playing); // 测试断言：要求 `history.files.size() >= 2 && !history.playing` 成立；失败立即终止测试。
    history.moves = loaded; history.playing = true; // 更新数据：`history.moves = loaded; history.playing = true`；赋值后的状态供后续逻辑或绘图使用。
    assert(!historyClick(history, 700, 330)); // 测试断言：要求 `!historyClick(history, 700, 330` 成立；失败立即终止测试。
    assert(history.replay.moves.size() == 1); // 测试断言：要求 `history.replay.moves.size() == 1` 成立；失败立即终止测试。
    assert(!historyClick(history, 590, 330)); // 测试断言：要求 `!historyClick(history, 590, 330` 成立；失败立即终止测试。
    assert(history.replay.moves.empty()); // 测试断言：要求 `history.replay.moves.empty(` 成立；失败立即终止测试。
    assert(!historyClick(history, 590, 330)); // 测试断言：要求 `!historyClick(history, 590, 330` 成立；失败立即终止测试。
    assert(history.replay.moves.empty()); // 测试断言：要求 `history.replay.moves.empty(` 成立；失败立即终止测试。
    for (std::size_t i = 0; i <= loaded.size(); ++i) historyClick(history, 700, 330); // 循环推进：`std::size_t i = 0; i <= loaded.size(); ++i) historyClick(history, 700, 330)`；逐项处理棋格、方向、字符或列表元素。
    assert(history.replay.moves.size() == loaded.size()); // 测试断言：要求 `history.replay.moves.size() == loaded.size(` 成立；失败立即终止测试。
    assert(!historyClick(history, 600, 390) && !history.playing); // 测试断言：要求 `!historyClick(history, 600, 390) && !history.playing` 成立；失败立即终止测试。

    history.files = {second}; history.page = 0; // 更新数据：`history.files = {second}; history.page = 0`；赋值后的状态供后续逻辑或绘图使用。
    assert(!historyClick(history, 150, 160) && history.canResume()); // 测试断言：要求 `!historyClick(history, 150, 160) && history.canResume(` 成立；失败立即终止测试。
    assert(!historyClick(history, 700, 330)); // 测试断言：要求 `!historyClick(history, 700, 330` 成立；失败立即终止测试。
    assert(history.replay.moves.size() == 1); // 测试断言：要求 `history.replay.moves.size() == 1` 成立；失败立即终止测试。
    assert(!historyClick(history, 600, 510) && history.resumeRequested); // 测试断言：要求 `!historyClick(history, 600, 510) && history.resumeRequested` 成立；失败立即终止测试。
    GameState continued = history.savedGame; // 声明 continued：测试中的续玩棋局；按右侧表达式初始化。
    assert(continued.moves.size() == 8 && continued.currentPlayer == BLACK_PIECE); // 测试断言：要求 `continued.moves.size() == 8 && continued.currentPlayer == BLACK_PIECE` 成立；失败立即终止测试。
    for (int r = 0; r < BOARD_SIZE; ++r) // 循环推进：`int r = 0; r < BOARD_SIZE; ++r)`；逐项处理棋格、方向、字符或列表元素。
        for (int c = 0; c < BOARD_SIZE; ++c) // 循环推进：`int c = 0; c < BOARD_SIZE; ++c)`；逐项处理棋格、方向、字符或列表元素。
            assert(continued.board[r][c] == game.board[r][c]); // 测试断言：要求 `continued.board[r][c] == game.board[r][c]` 成立；失败立即终止测试。
    assert(placePiece(continued, 4, 4)); // 测试断言：要求 `placePiece(continued, 4, 4` 成立；失败立即终止测试。
    std::wstring third; // 声明 third：第三份测试棋谱文件名；参数或长度由本行给出。
    assert(saveRecord(continued, third, error)); // 测试断言：要求 `saveRecord(continued, third, error` 成立；失败立即终止测试。
    history.playing = false; history.files = {third}; // 更新数据：`history.playing = false; history.files = {third}`；赋值后的状态供后续逻辑或绘图使用。
    assert(!historyClick(history, 150, 160) && history.canResume()); // 测试断言：要求 `!historyClick(history, 150, 160) && history.canResume(` 成立；失败立即终止测试。
    assert(history.savedGame.moves.size() == 9 && history.savedGame.currentPlayer == WHITE_PIECE); // 测试断言：要求 `history.savedGame.moves.size() == 9 && history.savedGame.currentPlayer == WHITE_PIECE` 成立；失败立即终止测试。
    continued = history.savedGame; // 跳过本轮剩余处理，继续下一条网络事件或循环元素。
    assert(undoLastMove(continued) && continued.currentPlayer == BLACK_PIECE); // 测试断言：要求 `undoLastMove(continued) && continued.currentPlayer == BLACK_PIECE` 成立；失败立即终止测试。
    assert(placePiece(continued, 0, 4) && continued.winner == BLACK_PIECE); // 测试断言：要求 `placePiece(continued, 0, 4) && continued.winner == BLACK_PIECE` 成立；失败立即终止测试。

    history.playing = false; history.files = {filename}; // 更新数据：`history.playing = false; history.files = {filename}`；赋值后的状态供后续逻辑或绘图使用。
    assert(!historyClick(history, 150, 160) && !history.canResume()); // 测试断言：要求 `!historyClick(history, 150, 160) && !history.canResume(` 成立；失败立即终止测试。
    assert(!historyClick(history, 600, 510) && !history.resumeRequested); // 测试断言：要求 `!historyClick(history, 600, 510) && !history.resumeRequested` 成立；失败立即终止测试。
    assert(!historyClick(history, 700, 330)); // 测试断言：要求 `!historyClick(history, 700, 330` 成立；失败立即终止测试。
    assert(!history.canResume()); // 测试断言：要求 `!history.canResume(` 成立；失败立即终止测试。
    history.savedGame.winner = EMPTY; // 更新数据：`history.savedGame.winner = EMPTY`；赋值后的状态供后续逻辑或绘图使用。
    history.savedGame.moves.resize(BOARD_SIZE * BOARD_SIZE); // 调整容器长度：`history.savedGame.moves.resize(BOARD_SIZE * BOARD_SIZE)`。
    assert(!history.canResume()); // 测试断言：要求 `!history.canResume(` 成立；失败立即终止测试。
    history.refresh(); // 执行 `history.refresh()`；调用相应对象的方法完成本步骤。
    assert(!history.resumeRequested && history.savedGame.moves.empty()); // 测试断言：要求 `!history.resumeRequested && history.savedGame.moves.empty(` 成立；失败立即终止测试。
    assert(DeleteFileW((recordDirectory() + L"\\" + third).c_str())); // 测试断言：要求 `DeleteFileW((recordDirectory() + L"\\" + third).c_str(` 成立；失败立即终止测试。

    std::wstring named, duplicate; // 声明 named：自定义名称保存后的文件名；参数或长度由本行给出。
    assert(saveRecord(game, named, error, L"  \u548c\u540c\u5b66\u7684\u5bf9\u5c40.gmk  ")); // 测试断言：要求 `saveRecord(game, named, error, L"  \u548c\u540c\u5b66\u7684\u5bf9\u5c40.gmk  "` 成立；失败立即终止测试。
    assert(named == L"\u548c\u540c\u5b66\u7684\u5bf9\u5c40.gmk"); // 测试断言：要求 `named == L"\u548c\u540c\u5b66\u7684\u5bf9\u5c40.gmk"` 成立；失败立即终止测试。
    assert(saveRecord(game, duplicate, error, L"\u548c\u540c\u5b66\u7684\u5bf9\u5c40")); // 测试断言：要求 `saveRecord(game, duplicate, error, L"\u548c\u540c\u5b66\u7684\u5bf9\u5c40"` 成立；失败立即终止测试。
    assert(duplicate != named); // 测试断言：要求 `duplicate != named` 成立；失败立即终止测试。
    assert(loadRecord(named, loaded, error) && loaded.size() == game.moves.size()); // 测试断言：要求 `loadRecord(named, loaded, error) && loaded.size() == game.moves.size(` 成立；失败立即终止测试。
    for (const wchar_t* bad : {L"../outside", L"CON", L"bad:name", L"name.", L"LPT1.gmk", L"bad\nname"}) // 循环推进：`const wchar_t* bad : {L"../outside", L"CON", L"bad:name", L"name.", L"LPT1.gmk", L"bad\nname"})`；逐项处理棋格、方向、字符或列表元素。
        assert(!saveRecord(game, third, error, bad) && third.empty()); // 测试断言：要求 `!saveRecord(game, third, error, bad) && third.empty(` 成立；失败立即终止测试。
    assert(!saveRecord(game, third, error, std::wstring(81, L'a'))); // 测试断言：要求 `!saveRecord(game, third, error, std::wstring(81, L'a'` 成立；失败立即终止测试。
    assert(DeleteFileW((recordDirectory() + L"\\" + named).c_str())); // 测试断言：要求 `DeleteFileW((recordDirectory() + L"\\" + named).c_str(` 成立；失败立即终止测试。
    assert(DeleteFileW((recordDirectory() + L"\\" + duplicate).c_str())); // 测试断言：要求 `DeleteFileW((recordDirectory() + L"\\" + duplicate).c_str(` 成立；失败立即终止测试。

    const char* invalid[] = { // 声明 invalid：是否含非法文件名内容；按右侧表达式初始化。
        "WUZIQI 2\n13\n0 0 1\n", "WUZIQI 1\n15\n0 0 1\n", // 承接上方表达式的参数、条件或初值：`"WUZIQI 2\n13\n0 0 1\n", "WUZIQI 1\n15\n0 0 1\n",`。
        "WUZIQI 1\n13\n13 0 1\n", "WUZIQI 1\n13\n0 0 2\n", // 承接上方表达式的参数、条件或初值：`"WUZIQI 1\n13\n13 0 1\n", "WUZIQI 1\n13\n0 0 2\n",`。
        "WUZIQI 1\n13\n0 0 1\n0 0 2\n", "WUZIQI 1\n13\n0 0 1\nx", // 承接上方表达式的参数、条件或初值：`"WUZIQI 1\n13\n0 0 1\n0 0 2\n", "WUZIQI 1\n13\n0 0 1\nx",`。
        "WUZIQI 1\n13\n0 0", "WUZIQI 1\n13\n" // 承接上方表达式的参数、条件或初值：`"WUZIQI 1\n13\n0 0", "WUZIQI 1\n13\n"`。
    };
    for (const char* data : invalid) // 循环推进：`const char* data : invalid)`；逐项处理棋格、方向、字符或列表元素。
    { // 开始上方函数、条件、循环或类型的作用域。
        replaceContents(second, data); // 仅测试使用：改写临时棋谱为非法内容，检查读取是否能拒绝。
        assert(!loadRecord(second, loaded, error)); // 测试断言：要求 `!loadRecord(second, loaded, error` 成立；失败立即终止测试。
        assert(loaded.size() == 8); // 失败时保留调用者已有棋谱。
    }
    assert(!loadRecord(L"..\\outside.gmk", loaded, error)); // 测试断言：要求 `!loadRecord(L"..\\outside.gmk", loaded, error` 成立；失败立即终止测试。
    assert(DeleteFileW((recordDirectory() + L"\\" + filename).c_str())); // 测试断言：要求 `DeleteFileW((recordDirectory() + L"\\" + filename).c_str(` 成立；失败立即终止测试。
    assert(DeleteFileW((recordDirectory() + L"\\" + second).c_str())); // 测试断言：要求 `DeleteFileW((recordDirectory() + L"\\" + second).c_str(` 成立；失败立即终止测试。
    std::cout << "Record and replay tests passed\n"; // 输出测试执行结果，便于确认测试是否运行到末尾。
}
