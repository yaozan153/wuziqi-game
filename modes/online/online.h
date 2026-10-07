// 文件职责：联机业务及界面：房间、IP 编辑、选色和棋步验证；底层传输交给 NetworkConnection。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#ifndef WUZIQI_ONLINE_H // 头文件防重复包含：只有尚未定义该标记时才展开下面的声明。
#define WUZIQI_ONLINE_H // 定义本头文件的包含标记，防止同一编译单元重复声明。
#include "network.h" // 引入 "network.h"，提供本文件使用的类型和函数声明。
#include "../../game.h" // 引入 "../../game.h"，提供本文件使用的类型和函数声明。

// main 保存会话，每帧 update；退出联机页时 reset。
struct OnlineSession // 定义 OnlineSession，集中组织相关数据。
{ // 开始上方函数、条件、循环或类型的作用域。
    NetworkConnection network; // 承接上方表达式的参数、条件或初值：`NetworkConnection network`。
    GameState game; // 声明 game：当前棋局；参数或长度由本行给出。
    PieceColor localPiece = EMPTY; // 声明 localPiece：本机玩家执棋颜色；按右侧表达式初始化。
    std::string serverIp; // 声明 serverIp：输入的房主 IPv4 地址；参数或长度由本行给出。
    bool showOtherAddresses = false; // 声明 showOtherAddresses：是否展开其他网卡地址；按右侧表达式初始化。
    bool editingIp = false; // 声明 editingIp：是否正在编辑 IP；按右侧表达式初始化。
    std::size_t ipCursor = 0; // 声明 ipCursor：IP 输入框中的字符光标位置；按右侧表达式初始化。
    bool inRoom = false; // 声明 inRoom：是否已发起创建或加入房间；按右侧表达式初始化。
    bool isHost = false; // 声明 isHost：本机是否为房主；按右侧表达式初始化。
    bool choicePending = false; // 声明 choicePending：加入方是否正在等待房主确认颜色；按右侧表达式初始化。
    bool create(unsigned short port = 8888); // 声明接口：重置会话、标记房主并调用网络监听。
    bool join(unsigned short port = 8888); // 声明接口：网络层解析 IPv4 并非阻塞连接；联机业务层使用输入框 IP 加入房间。
    void reset(); // 声明接口：关闭联机连接并清空棋局和选色状态，保留输入的服务器 IP。
    void update(); // 声明接口：推进网络并按顺序处理选色与棋步，拒绝非法回合和错误步号。
    bool move(int row, int col); // 声明接口：仅本方回合允许落子，先加入发送队列成功后再更新棋盘。
    bool chooseColor(PieceColor piece); // 声明接口：房主直接确认颜色，加入方先发送请求，确认后双方自动取相反颜色。
};
void drawOnlineScreen(const OnlineSession& session, int mouseX, int mouseY); // 声明接口：根据大厅、等待房间或已选色对局状态绘制对应联机界面。
// true 表示返回菜单，同时关闭连接。
bool onlineClick(OnlineSession& session, int x, int y); // 声明接口：处理联机界面的返回、创建、加入、选色、地址展开和棋盘点击。
// IP 输入框接受数字、点、退格，Enter 加入；扩展编辑键为 256 + 扫描码。
void onlineKey(OnlineSession& session, int key); // 声明接口：编辑 IPv4 输入框光标及数字点号，Enter 发起加入。
#endif // 结束头文件防重复包含的条件范围。
