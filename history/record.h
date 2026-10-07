// 文件职责：棋谱持久化：仅手动保存时写入 .gmk，读取时逐步校验，不负责页面切换。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#ifndef WUZIQI_RECORD_H // 头文件防重复包含：只有尚未定义该标记时才展开下面的声明。
#define WUZIQI_RECORD_H // 定义本头文件的包含标记，防止同一编译单元重复声明。
#include "../game.h" // 引入 "../game.h"，提供本文件使用的类型和函数声明。
#include <string> // 引入 <string>，提供本文件使用的类型和函数声明。

std::wstring recordDirectory(); // 声明接口：从程序路径取得旁边的 records 文件夹绝对路径。
bool saveRecord(const GameState& game, std::wstring& filename, std::wstring& error, // 函数入口：校验棋步及名称，用 CREATE_NEW 新建棋谱，同名加后缀，写入成功才返回 true。
                const std::wstring& name = L""); // 声明 name：用户输入的棋谱名称；按右侧表达式初始化。
bool loadRecord(const std::wstring& filename, std::vector<Move>& moves, std::wstring& error); // 声明接口：读取小型棋谱文件，检查版本和尺寸，逐步校验后一次性返回全部棋步。
#endif // 结束头文件防重复包含的条件范围。
