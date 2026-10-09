// 文件职责：非阻塞 TCP 网络层：套接字、10 秒连接握手超时、固定六字节协议和收发缓冲。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#ifndef WUZIQI_NETWORK_H // 头文件防重复包含：只有尚未定义该标记时才展开下面的声明。
#define WUZIQI_NETWORK_H // 定义本头文件的包含标记，防止同一编译单元重复声明。
// 公共头文件不引入 Winsock，避免与 EGE 的 windows.h 包含顺序冲突。
#include <cstdint> // 引入 <cstdint>，提供本文件使用的类型和函数声明。
#include <string> // 引入 <string>，提供本文件使用的类型和函数声明。
#include <deque> // 引入 <deque>，提供本文件使用的类型和函数声明。
#include <chrono> // 引入 <chrono>，提供本文件使用的类型和函数声明。
#include <vector> // 引入 <vector>，提供本文件使用的类型和函数声明。

enum class NetworkState { IDLE, LISTENING, CONNECTING, HANDSHAKING, CONNECTED, DISCONNECTED, FAILED }; // 定义离散状态取值：`enum class NetworkState { IDLE, LISTENING, CONNECTING, HANDSHAKING, CONNECTED, DISCONNECTED, FAILED }`。
struct NetworkMove { int row, col, sequence, type; }; // 定义 NetworkMove，集中组织相关数据。

// 单线程非阻塞连接；每帧调用 poll。不拥有棋盘，不涉及绘图。
class NetworkConnection // 定义 NetworkConnection，集中组织相关数据和方法。
{ // 开始上方函数、条件、循环或类型的作用域。
public: // 以下成员可供外部调用。
    NetworkConnection(); // 初始化 Winsock 并将套接字设为无效，或析构时释放套接字和 Winsock。
    ~NetworkConnection(); // 执行 `~NetworkConnection()`；调用相应对象的方法完成本步骤。
    NetworkConnection(const NetworkConnection&) = delete; // 禁止复制连接对象，避免多个对象重复关闭同一个套接字。
    NetworkConnection& operator=(const NetworkConnection&) = delete; // 禁止复制连接对象，避免多个对象重复关闭同一个套接字。
    bool host(unsigned short port = 18888); // 声明接口：独占指定端口监听一个玩家，并查询本机可用 IPv4 地址供加入方填写。
    bool join(const std::string& ipv4, unsigned short port = 18888); // 声明接口：网络层解析 IPv4 并非阻塞连接；联机业务层使用输入框 IP 加入房间。
    void poll(); // 声明接口：每帧推进连接、发送未发完数据、接收及拆出完整六字节消息。
    bool sendMove(int row, int col, int sequence); // 声明接口：校验坐标和步号，将六字节棋步消息加入发送队列，不代表对方已确认。
    bool sendColor(int piece, bool assignment); // 声明接口：将选色请求或房主确认加入发送队列，类型分别为 2 和 3。
    bool receiveMove(NetworkMove& move); // 声明接口：按收到顺序取出一条已解析事件，兼容棋步与选色事件。
    void close(); // 声明接口：关闭监听和对端套接字，清理收发缓存及网络状态。
    void reject(const std::string& reason); // 声明接口：关闭连接并保存失败原因，避免继续处理异常网络数据。
    NetworkState state() const { return state_; } // 承接上方表达式的参数、条件或初值：`NetworkState state() const { return state_; }`。
    const std::string& error() const { return error_; } // 声明 error：错误原因；参数或长度由本行给出。
    const std::vector<std::string>& hostAddresses() const { return hostAddresses_; } // 声明 hostAddresses：本行使用的局部数据；参数或长度由本行给出。
private: // 以下成员仅类内部可访问，用于封装网络状态。
    bool startup_; // 声明 startup_：Winsock 是否初始化成功；参数或长度由本行给出。
    std::uintptr_t listener_, peer_; // 声明 listener_：监听套接字；参数或长度由本行给出。
    NetworkState state_; // 承接上方表达式的参数、条件或初值：`NetworkState state_`。
    std::string error_, outgoing_, incoming_; // 声明 error_：网络错误说明；参数或长度由本行给出。
    std::deque<NetworkMove> moves_; // 声明 moves_：按顺序解析出的网络事件队列；参数或长度由本行给出。
    std::vector<std::string> hostAddresses_; // 声明 hostAddresses_：本机可用房主 IP 列表；参数或长度由本行给出。
    std::chrono::steady_clock::time_point started_; // 声明 started_：连接或握手起始时刻；参数或长度由本行给出。
    bool makeSocket(std::uintptr_t& socket); // 声明接口：创建 IPv4 TCP 套接字并启用非阻塞模式，避免卡住游戏绘图。
    void handshake(); // 声明接口：进入握手状态并把六字节协议问候包放入发送缓冲。
};
#endif // 结束头文件防重复包含的条件范围。
