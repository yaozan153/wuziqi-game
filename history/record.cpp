// 文件职责：棋谱持久化：仅手动保存时写入 .gmk，读取时逐步校验，不负责页面切换。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#include "record.h" // 引入 "record.h"，提供本文件使用的类型和函数声明。
#include <windows.h> // 引入 <windows.h>，提供本文件使用的类型和函数声明。
#include <sstream> // 引入 <sstream>，提供本文件使用的类型和函数声明。
#include <cwctype> // 引入 <cwctype>，提供本文件使用的类型和函数声明。

std::wstring recordDirectory() // 函数入口：从程序路径取得旁边的 records 文件夹绝对路径。
{ // 开始上方函数、条件、循环或类型的作用域。
    wchar_t path[32768] = {}; // 为当前可执行文件的Unicode路径准备32768字符缓冲，先全部清零。
    const DWORD length = GetModuleFileNameW(NULL, path, 32768); // 读取本程序路径，length是实际取得的路径字符数，0表示失败。
    if (!length || length >= 32768) return L""; // 检查 `if (!length || length >= 32768) return L""`；条件成立时执行括号之后或下一行的处理。
    const std::wstring executable(path, length); // 声明 executable：本行使用的局部数据；参数或长度由本行给出。
    return executable.substr(0, executable.find_last_of(L"\\/")) + L"\\records"; // 返回 `executable.substr(0, executable.find_last_of(L"\\/")) + L"\\records"`，将结果交给调用者。
}

namespace // 开启匿名命名空间，让内部辅助函数只在当前源文件可见。
{ // 开始上方函数、条件、循环或类型的作用域。
    bool validMoves(const std::vector<Move>& moves) // 函数入口：从新棋局逐步重放，确认颜色、回合、位置及获胜后没有继续落子。
    { // 开始上方函数、条件、循环或类型的作用域。
        GameState checked; // 声明 checked：校验棋步时使用的独立棋局；参数或长度由本行给出。
        for (const Move& move : moves) // 循环推进：`const Move& move : moves)`；逐项处理棋格、方向、字符或列表元素。
            if (move.piece != checked.currentPlayer || !placePiece(checked, move.row, move.col)) // 检查 `if (move.piece != checked.currentPlayer || !placePiece(checked, move.row, move.col))`；条件成立时执行括号之后或下一行的处理。
                return false; // 返回 false：本次条件不满足或操作未成功；具体含义见当前函数说明。
        return true; // 返回 true：本次条件成立或操作成功；具体含义见当前函数说明。
    }
}

bool saveRecord(const GameState& game, std::wstring& filename, std::wstring& error, // 函数入口：校验棋步及名称，用 CREATE_NEW 新建棋谱，同名加后缀，写入成功才返回 true。
                const std::wstring& name) // 声明 name：用户输入的棋谱名称；参数或长度由本行给出。
{ // 开始上方函数、条件、循环或类型的作用域。
    filename.clear(); error.clear(); // 清空 `filename.clear(); error.clear()`，避免旧状态残留。
    if (game.moves.empty()) { error = L"尚未落子，无法保存棋谱"; return false; } // 检查 `if (game.moves.empty()) { error = L"尚未落子，无法保存棋谱"; return false; }`；条件成立时执行括号之后或下一行的处理。
    if (!validMoves(game.moves)) { error = L"棋步记录异常，无法保存"; return false; } // 检查 `if (!validMoves(game.moves)) { error = L"棋步记录异常，无法保存"; return false; }`；条件成立时执行括号之后或下一行的处理。
    std::wstring title = name; // 声明 title：标题文字或处理后的存档名称；按右侧表达式初始化。
    const std::size_t first = title.find_first_not_of(L" \t\r\n"); // 声明 first：首个非空白字符的位置；按右侧表达式初始化。
    title = first == std::wstring::npos ? L"" : title.substr(first, title.find_last_not_of(L" \t\r\n") - first + 1); // 更新数据：`title = first == std::wstring::npos ? L"" : title.substr(first, title.find_last_not_of(L" \t\r\n") - first + 1)`；赋值后的状态供后续逻辑或绘图使用。
    std::wstring upper = title; // 声明 upper：用于比较扩展名和保留名的大写名称；按右侧表达式初始化。
    for (wchar_t& ch : upper) ch = std::towupper(ch); // 循环推进：`wchar_t& ch : upper) ch = std::towupper(ch)`；逐项处理棋格、方向、字符或列表元素。
    if (upper.size() >= 4 && upper.substr(upper.size() - 4) == L".GMK") // 检查 `if (upper.size() >= 4 && upper.substr(upper.size() - 4) == L".GMK")`；条件成立时执行括号之后或下一行的处理。
    { title.resize(title.size() - 4); upper.resize(upper.size() - 4); } // 调整容器长度：`{ title.resize(title.size() - 4); upper.resize(upper.size() - 4); }`。
    const std::wstring device = upper.substr(0, upper.find(L'.')); // 声明 device：点号前的文件名部分；按右侧表达式初始化。
    const bool reserved = device == L"CON" || device == L"PRN" || device == L"AUX" || device == L"NUL" || // 声明 reserved：是否为 Windows 系统保留文件名；按右侧表达式初始化。
        (device.size() == 4 && (device.substr(0, 3) == L"COM" || device.substr(0, 3) == L"LPT") && // 继续上方条件：`(device.size() == 4 && (device.substr(0, 3) == L"COM" || device.substr(0, 3) == L"LPT") &&`；&& 要求同时满足，|| 表示任一成立。
         device[3] >= L'1' && device[3] <= L'9'); // 承接上方表达式的参数、条件或初值：`device[3] >= L'1' && device[3] <= L'9')`。
    bool invalid = title.size() > 80 || title.find_first_of(L"\\/:*?\"<>|") != std::wstring::npos || reserved; // 声明 invalid：是否含非法文件名内容；按右侧表达式初始化。
    for (wchar_t ch : title) if (ch < 32) invalid = true; // 循环推进：`wchar_t ch : title) if (ch < 32) invalid = true`；逐项处理棋格、方向、字符或列表元素。
    if (!title.empty() && (title.back() == L'.' || title.back() == L' ')) invalid = true; // 检查 `if (!title.empty() && (title.back() == L'.' || title.back() == L' ')) invalid = true`；条件成立时执行括号之后或下一行的处理。
    if (invalid) { error = L"名称须在 80 字以内，不能包含 \\/:*?\"<>|、以空格或句点结尾，或使用系统保留名称"; return false; } // 检查 `if (invalid) { error = L"名称须在 80 字以内，不能包含 \\/:*?\"<>|、以空格或句点结尾，或使用系统保留名称"; return false; }`；条件成立时执行括号之后或下一行的处理。
    const std::wstring directory = recordDirectory(); // 声明 directory：目标文件夹路径；按右侧表达式初始化。
    if (directory.empty() || (!CreateDirectoryW(directory.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)) // 检查 `if (directory.empty() || (!CreateDirectoryW(directory.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS))`；条件成立时执行括号之后或下一行的处理。
    { error = L"无法创建 records 文件夹"; return false; } // 更新数据：`{ error = L"无法创建 records 文件夹"; return false; }`；赋值后的状态供后续逻辑或绘图使用。
    SYSTEMTIME time; GetLocalTime(&time); // 声明 time：本地保存时间；参数或长度由本行给出。
    wchar_t stamp[64]; // 声明 stamp：按保存时间生成的默认名称；参数或长度由本行给出。
    swprintf(stamp, 64, L"%04u-%02u-%02u_%02u-%02u-%02u", time.wYear, time.wMonth, // 格式化默认时间文件名；参数为 `stamp, 64, L"%04u-%02u-%02u_%02u-%02u-%02u", time.wYear, time.wMonth,`。
             time.wDay, time.wHour, time.wMinute, time.wSecond); // 承接上方表达式的参数、条件或初值：`time.wDay, time.wHour, time.wMinute, time.wSecond)`。
    if (title.empty()) title = stamp; // 名称留空或只有空白时，回退到保存时间作为文件名。
    HANDLE file = INVALID_HANDLE_VALUE; // 声明 file：打开文件的 Windows 句柄；按右侧表达式初始化。
    for (int suffix = 0; suffix < 10000; ++suffix) // 循环推进：`int suffix = 0; suffix < 10000; ++suffix)`；逐项处理棋格、方向、字符或列表元素。
    { // 开始上方函数、条件、循环或类型的作用域。
        filename = title + (suffix ? L"_" + std::to_wstring(suffix) : L"") + L".gmk"; // 更新数据：`filename = title + (suffix ? L"_" + std::to_wstring(suffix) : L"") + L".gmk"`；赋值后的状态供后续逻辑或绘图使用。
        file = CreateFileW((directory + L"\\" + filename).c_str(), GENERIC_WRITE, 0, NULL, // 更新数据：`file = CreateFileW((directory + L"\\" + filename).c_str(), GENERIC_WRITE, 0, NULL,`；赋值后的状态供后续逻辑或绘图使用。
                           CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL); // 仅创建新文件：同名已有文件会失败，外层随后尝试数字后缀，绝不覆盖。
        if (file != INVALID_HANDLE_VALUE || GetLastError() != ERROR_FILE_EXISTS) break; // 检查 `if (file != INVALID_HANDLE_VALUE || GetLastError() != ERROR_FILE_EXISTS) break`；条件成立时执行括号之后或下一行的处理。
    }
    if (file == INVALID_HANDLE_VALUE) { error = L"无法保存棋谱，请检查文件夹权限"; return false; } // 检查 `if (file == INVALID_HANDLE_VALUE) { error = L"无法保存棋谱，请检查文件夹权限"; return false; }`；条件成立时执行括号之后或下一行的处理。
    std::ostringstream stream; // 声明 stream：格式化读写数据的流；参数或长度由本行给出。
    stream << "WUZIQI 1\n" << BOARD_SIZE << '\n'; // 承接上方表达式的参数、条件或初值：`stream << "WUZIQI 1\n" << BOARD_SIZE << '\n'`。
    for (const Move& move : game.moves) // 循环推进：`const Move& move : game.moves)`；逐项处理棋格、方向、字符或列表元素。
        stream << move.row << ' ' << move.col << ' ' << static_cast<int>(move.piece) << '\n'; // 执行 `stream << move.row << ' ' << move.col << ' ' << static_cast<int>(move.piece) << '\n'`；调用相应对象的方法完成本步骤。
    const std::string data = stream.str(); // 声明 data：待读写数据；按右侧表达式初始化。
    DWORD written = 0; // 声明 written：实际写入字节数；按右侧表达式初始化。
    const bool success = WriteFile(file, data.data(), static_cast<DWORD>(data.size()), &written, NULL) // 声明 success：本行使用的局部数据；按右侧表达式初始化。
                         && written == data.size(); // 继续上方条件：`&& written == data.size()`；&& 要求同时满足，|| 表示任一成立。
    const bool flushed = success && FlushFileBuffers(file); // 声明 flushed：本行使用的局部数据；按右侧表达式初始化。
    CloseHandle(file); // 释放文件或搜索句柄；参数为 `file)`。
    if (!flushed) // 检查 `if (!flushed)`；条件成立时执行括号之后或下一行的处理。
    { // 开始上方函数、条件、循环或类型的作用域。
        DeleteFileW((directory + L"\\" + filename).c_str()); // 删除写入失败的新文件或测试自己生成的临时棋谱；参数为 `(directory + L"\\" + filename).c_str())`。
        error = L"棋谱写入失败"; filename.clear(); return false; // 更新数据：`error = L"棋谱写入失败"; filename.clear(); return false`；赋值后的状态供后续逻辑或绘图使用。
    }
    return true; // 返回 true：本次条件成立或操作成功；具体含义见当前函数说明。
}

bool loadRecord(const std::wstring& filename, std::vector<Move>& moves, std::wstring& error) // 函数入口：读取小型棋谱文件，检查版本和尺寸，逐步校验后一次性返回全部棋步。
{ // 开始上方函数、条件、循环或类型的作用域。
    error.clear(); // 清空 `error.clear()`，避免旧状态残留。
    if (filename.empty() || filename.find_first_of(L"\\/:") != std::wstring::npos) // 检查 `if (filename.empty() || filename.find_first_of(L"\\/:") != std::wstring::npos)`；条件成立时执行括号之后或下一行的处理。
    { error = L"棋谱文件名无效"; return false; } // 更新数据：`{ error = L"棋谱文件名无效"; return false; }`；赋值后的状态供后续逻辑或绘图使用。
    HANDLE file = CreateFileW((recordDirectory() + L"\\" + filename).c_str(), GENERIC_READ, // 声明 file：打开文件的 Windows 句柄；按右侧表达式初始化。
                             FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); // 承接上方表达式的参数、条件或初值：`FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)`。
    if (file == INVALID_HANDLE_VALUE) { error = L"无法打开棋谱"; return false; } // 检查 `if (file == INVALID_HANDLE_VALUE) { error = L"无法打开棋谱"; return false; }`；条件成立时执行括号之后或下一行的处理。
    LARGE_INTEGER size; // 承接上方表达式的参数、条件或初值：`LARGE_INTEGER size`。
    if (!GetFileSizeEx(file, &size) || size.QuadPart <= 0 || size.QuadPart > 16384) // 检查 `if (!GetFileSizeEx(file, &size) || size.QuadPart <= 0 || size.QuadPart > 16384)`；条件成立时执行括号之后或下一行的处理。
    { CloseHandle(file); error = L"棋谱文件大小异常"; return false; } // 更新数据：`{ CloseHandle(file); error = L"棋谱文件大小异常"; return false; }`；赋值后的状态供后续逻辑或绘图使用。
    std::string data(static_cast<std::size_t>(size.QuadPart), '\0'); // 声明 data：待读写数据；参数或长度由本行给出。
    DWORD received = 0; // 声明 received：实际收到或读取的数据长度；按右侧表达式初始化。
    const bool success = ReadFile(file, &data[0], static_cast<DWORD>(data.size()), &received, NULL) // 声明 success：本行使用的局部数据；按右侧表达式初始化。
                         && received == data.size(); // 继续上方条件：`&& received == data.size()`；&& 要求同时满足，|| 表示任一成立。
    CloseHandle(file); // 释放文件或搜索句柄；参数为 `file)`。
    if (!success) { error = L"棋谱读取失败"; return false; } // 检查 `if (!success) { error = L"棋谱读取失败"; return false; }`；条件成立时执行括号之后或下一行的处理。
    std::istringstream stream(data); // 声明 stream：格式化读写数据的流；参数或长度由本行给出。
    std::string magic; // 声明 magic：本行使用的局部数据；参数或长度由本行给出。
    int version, boardSize; // 声明 version：本行使用的局部数据；参数或长度由本行给出。
    if (!(stream >> magic >> version >> boardSize) || magic != "WUZIQI" || version != 1 || boardSize != BOARD_SIZE) // 检查 `if (!(stream >> magic >> version >> boardSize) || magic != "WUZIQI" || version != 1 || boardSize != BOARD_SIZE)`；条件成立时执行括号之后或下一行的处理。
    { error = L"棋谱格式或棋盘大小不兼容"; return false; } // 更新数据：`{ error = L"棋谱格式或棋盘大小不兼容"; return false; }`；赋值后的状态供后续逻辑或绘图使用。
    GameState checked; // 声明 checked：校验棋步时使用的独立棋局；参数或长度由本行给出。
    while (stream >> std::ws && !stream.eof()) // 只要 `stream >> std::ws && !stream.eof())` 成立就继续处理；用于事件、连子或网络队列。
    { // 开始上方函数、条件、循环或类型的作用域。
        int row, col, color; // 声明 row：棋盘行号；参数或长度由本行给出。
        if (!(stream >> row >> col >> color) || color != checked.currentPlayer || !placePiece(checked, row, col)) // 检查 `if (!(stream >> row >> col >> color) || color != checked.currentPlayer || !placePiece(checked, row, col))`；条件成立时执行括号之后或下一行的处理。
        { error = L"棋谱含有无效棋步"; return false; } // 更新数据：`{ error = L"棋谱含有无效棋步"; return false; }`；赋值后的状态供后续逻辑或绘图使用。
    }
    if (checked.moves.empty()) { error = L"棋谱没有棋步"; return false; } // 检查 `if (checked.moves.empty()) { error = L"棋谱没有棋步"; return false; }`；条件成立时执行括号之后或下一行的处理。
    moves = checked.moves; // 全部内容校验成功后才替换调用者的棋步；读取失败不破坏旧数据。
    return true; // 返回 true：本次条件成立或操作成功；具体含义见当前函数说明。
}
