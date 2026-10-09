// 文件职责：非阻塞 TCP 网络层：套接字、10 秒连接握手超时、固定六字节协议和收发缓冲。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#include <winsock2.h> // 引入 <winsock2.h>，提供本文件使用的类型和函数声明。
#include <ws2tcpip.h> // 引入 <ws2tcpip.h>，提供本文件使用的类型和函数声明。
#include <iphlpapi.h> // 引入 <iphlpapi.h>，提供本文件使用的类型和函数声明。
#include <algorithm> // 引入 <algorithm>，提供本文件使用的类型和函数声明。
#include "network.h" // 引入 "network.h"，提供本文件使用的类型和函数声明。
#include "../../game.h" // 引入 "../../game.h"，提供本文件使用的类型和函数声明。

NetworkConnection::NetworkConnection() // 连接对象生命周期：初始化 Winsock 并将套接字设为无效，或析构时释放套接字和 Winsock。
    : startup_(false), listener_(INVALID_SOCKET), peer_(INVALID_SOCKET), state_(NetworkState::IDLE) // 构造函数初始化列表：设置网络库、套接字及状态的初始值。
{ // 开始上方函数、条件、循环或类型的作用域。
    WSADATA data; // 声明 data：待读写数据；参数或长度由本行给出。
    startup_ = WSAStartup(MAKEWORD(2, 2), &data) == 0; // 更新数据：`startup_ = WSAStartup(MAKEWORD(2, 2), &data) == 0`；赋值后的状态供后续逻辑或绘图使用。
}
NetworkConnection::~NetworkConnection() { close(); if (startup_) WSACleanup(); } // 连接对象生命周期：初始化 Winsock 并将套接字设为无效，或析构时释放套接字和 Winsock。

void NetworkConnection::close() // 函数入口：关闭监听和对端套接字，清理收发缓存及网络状态。
{ // 开始上方函数、条件、循环或类型的作用域。
    if (listener_ != INVALID_SOCKET) closesocket(listener_); // 检查 `if (listener_ != INVALID_SOCKET) closesocket(listener_)`；条件成立时执行括号之后或下一行的处理。
    if (peer_ != INVALID_SOCKET) closesocket(peer_); // 检查 `if (peer_ != INVALID_SOCKET) closesocket(peer_)`；条件成立时执行括号之后或下一行的处理。
    listener_ = peer_ = INVALID_SOCKET; // 更新数据：`listener_ = peer_ = INVALID_SOCKET`；赋值后的状态供后续逻辑或绘图使用。
    outgoing_.clear(); incoming_.clear(); moves_.clear(); error_.clear(); // 清空 `outgoing_.clear(); incoming_.clear(); moves_.clear(); error_.clear()`，避免旧状态残留。
    hostAddresses_.clear(); // 清空 `hostAddresses_.clear()`，避免旧状态残留。
    state_ = NetworkState::IDLE; // 更新数据：`state_ = NetworkState::IDLE`；赋值后的状态供后续逻辑或绘图使用。
}
void NetworkConnection::reject(const std::string& reason) // 函数入口：关闭连接并保存失败原因，避免继续处理异常网络数据。
{ // 开始上方函数、条件、循环或类型的作用域。
    close(); state_ = NetworkState::FAILED; error_ = reason; // 关闭监听和对端套接字，清理收发缓存及网络状态。
}
bool NetworkConnection::makeSocket(std::uintptr_t& socket) // 函数入口：创建 IPv4 TCP 套接字并启用非阻塞模式，避免卡住游戏绘图。
{ // 开始上方函数、条件、循环或类型的作用域。
    if (!startup_) { reject("Winsock initialization failed"); return false; } // 检查 `if (!startup_) { reject("Winsock initialization failed"); return false; }`；条件成立时执行括号之后或下一行的处理。
    socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 更新数据：`socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)`；赋值后的状态供后续逻辑或绘图使用。
    u_long nonblocking = 1; // 声明 nonblocking：启用非阻塞模式的标志；按右侧表达式初始化。
    if (socket == INVALID_SOCKET || ioctlsocket(socket, FIONBIO, &nonblocking) != 0) // 检查 `if (socket == INVALID_SOCKET || ioctlsocket(socket, FIONBIO, &nonblocking) != 0)`；条件成立时执行括号之后或下一行的处理。
    { reject("Cannot create nonblocking socket"); return false; } // 拒绝连接并记录这里的错误原因，随后提前返回，防止继续使用异常连接。
    return true; // 返回 true：本次条件成立或操作成功；具体含义见当前函数说明。
}
bool NetworkConnection::host(unsigned short port) // 函数入口：独占指定端口监听一个玩家，并查询本机可用 IPv4 地址供加入方填写。
{ // 开始上方函数、条件、循环或类型的作用域。
    close(); // 关闭监听和对端套接字，清理收发缓存及网络状态。
    if (!makeSocket(listener_)) return false; // 检查 `if (!makeSocket(listener_)) return false`；条件成立时执行括号之后或下一行的处理。
    // 独占端口，避免第二个创建窗口抢用同一端口。
    BOOL exclusive = TRUE; // 声明 exclusive：独占监听端口的标志；按右侧表达式初始化。
    setsockopt(listener_, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, // 设置套接字选项；参数为 `listener_, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,`。
               reinterpret_cast<const char*>(&exclusive), sizeof(exclusive)); // 执行 `reinterpret_cast<const char*>(&exclusive), sizeof(exclusive))`；调用相应对象的方法完成本步骤。
    sockaddr_in address; // 声明 address：IPv4 地址结构或地址文字；参数或长度由本行给出。
    ZeroMemory(&address, sizeof(address)); // 将 Windows 结构清零；参数为 `&address, sizeof(address))`。
    address.sin_family = AF_INET; // 更新数据：`address.sin_family = AF_INET`；赋值后的状态供后续逻辑或绘图使用。
    address.sin_addr.s_addr = htonl(INADDR_ANY); // 更新数据：`address.sin_addr.s_addr = htonl(INADDR_ANY)`；赋值后的状态供后续逻辑或绘图使用。
    address.sin_port = htons(port); // 更新数据：`address.sin_port = htons(port)`；赋值后的状态供后续逻辑或绘图使用。
    if (bind(listener_, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0)
    {
        const int error = WSAGetLastError(); // 在关闭套接字前保留系统错误码。
        reject("Cannot bind port " + std::to_string(port) + " (Winsock " + std::to_string(error) + ")");
        return false;
    }
    if (listen(listener_, 1) != 0)
    {
        const int error = WSAGetLastError();
        reject("Cannot listen (Winsock " + std::to_string(error) + ")");
        return false;
    }
    state_ = NetworkState::LISTENING; // 更新数据：`state_ = NetworkState::LISTENING`；赋值后的状态供后续逻辑或绘图使用。
    // Query local interfaces without DNS or an external service.
    std::vector<INTERFACE_INFO> interfaces(16); // 声明 interfaces：本机网络接口信息列表；参数或长度由本行给出。
    DWORD bytes = 0; // 声明 bytes：查询网卡实际返回的字节数；按右侧表达式初始化。
    int result; // 声明 result：本次操作结果；参数或长度由本行给出。
    do // 先执行一次循环体，再在末尾判断是否需要重试。
    { // 开始上方函数、条件、循环或类型的作用域。
        result = WSAIoctl(listener_, SIO_GET_INTERFACE_LIST, NULL, 0, // 更新数据：`result = WSAIoctl(listener_, SIO_GET_INTERFACE_LIST, NULL, 0,`；赋值后的状态供后续逻辑或绘图使用。
                          interfaces.data(), interfaces.size() * sizeof(INTERFACE_INFO), // 承接上方表达式的参数、条件或初值：`interfaces.data(), interfaces.size() * sizeof(INTERFACE_INFO),`。
                          &bytes, NULL, NULL); // 承接上方表达式的参数、条件或初值：`&bytes, NULL, NULL)`。
        if (result == 0 || WSAGetLastError() != WSAEFAULT || interfaces.size() >= 1024) break; // 检查 `if (result == 0 || WSAGetLastError() != WSAEFAULT || interfaces.size() >= 1024) break`；条件成立时执行括号之后或下一行的处理。
        interfaces.resize(interfaces.size() * 2); // 调整容器长度：`interfaces.resize(interfaces.size() * 2)`。
    } while (true); // 本轮完成后再次检查循环条件，直到内部 break 结束。
    if (result == 0) // 检查 `if (result == 0)`；条件成立时执行括号之后或下一行的处理。
    { // 开始上方函数、条件、循环或类型的作用域。
        for (std::size_t i = 0; i < bytes / sizeof(INTERFACE_INFO); ++i) // 循环推进：`std::size_t i = 0; i < bytes / sizeof(INTERFACE_INFO); ++i)`；逐项处理棋格、方向、字符或列表元素。
        { // 开始上方函数、条件、循环或类型的作用域。
            const INTERFACE_INFO& info = interfaces[i]; // 更新数据：`const INTERFACE_INFO& info = interfaces[i]`；赋值后的状态供后续逻辑或绘图使用。
            const unsigned long ip = ntohl(info.iiAddress.AddressIn.sin_addr.s_addr); // 声明 ip：IPv4 数值或显示文字；按右侧表达式初始化。
            if (!(info.iiFlags & IFF_UP) || (info.iiFlags & IFF_LOOPBACK) || // 检查 `if (!(info.iiFlags & IFF_UP) || (info.iiFlags & IFF_LOOPBACK) ||`；条件成立时执行括号之后或下一行的处理。
                info.iiAddress.Address.sa_family != AF_INET || !ip || // 继续上方条件：`info.iiAddress.Address.sa_family != AF_INET || !ip ||`；&& 要求同时满足，|| 表示任一成立。
                (ip >> 24) == 127 || (ip >> 16) == 0xa9fe) continue; // 执行 `(ip >> 24) == 127 || (ip >> 16) == 0xa9fe) continue`；调用相应对象的方法完成本步骤。
            const std::string addressText = inet_ntoa(info.iiAddress.AddressIn.sin_addr); // 声明 addressText：本行使用的局部数据；按右侧表达式初始化。
            if (std::find(hostAddresses_.begin(), hostAddresses_.end(), addressText) == hostAddresses_.end()) // 检查 `if (std::find(hostAddresses_.begin(), hostAddresses_.end(), addressText) == hostAddresses_.end())`；条件成立时执行括号之后或下一行的处理。
                hostAddresses_.push_back(addressText); // 追加一项到 `hostAddresses_`，保留数据的先后顺序。
        }
    }
    // Prefer the interface used by the system's normal outbound route.
    // This only queries the local routing table; no packet is sent.
    DWORD bestIndex = 0; // 声明 bestIndex：默认出站路由对应的网卡索引；按右侧表达式初始化。
    ULONG size = 0; // 声明 size：查询或缓冲区大小；按右侧表达式初始化。
    if (GetBestInterface(inet_addr("1.1.1.1"), &bestIndex) == NO_ERROR && // 检查 `if (GetBestInterface(inet_addr("1.1.1.1"), &bestIndex) == NO_ERROR &&`；条件成立时执行括号之后或下一行的处理。
        GetAdaptersInfo(NULL, &size) == ERROR_BUFFER_OVERFLOW) // 查询本机网卡及其地址；参数为 `NULL, &size) == ERROR_BUFFER_OVERFLOW)`。
    { // 开始上方函数、条件、循环或类型的作用域。
        std::vector<unsigned long long> buffer((size + 7) / 8); // 按八字节元素申请足够大的网卡信息缓冲，向上取整避免空间不足。
        IP_ADAPTER_INFO* adapters = reinterpret_cast<IP_ADAPTER_INFO*>(buffer.data()); // 更新数据：`IP_ADAPTER_INFO* adapters = reinterpret_cast<IP_ADAPTER_INFO*>(buffer.data())`；赋值后的状态供后续逻辑或绘图使用。
        if (GetAdaptersInfo(adapters, &size) == NO_ERROR) // 检查 `if (GetAdaptersInfo(adapters, &size) == NO_ERROR)`；条件成立时执行括号之后或下一行的处理。
        { // 开始上方函数、条件、循环或类型的作用域。
            for (IP_ADAPTER_INFO* adapter = adapters; adapter; adapter = adapter->Next) // 循环推进：`IP_ADAPTER_INFO* adapter = adapters; adapter; adapter = adapter->Next)`；逐项处理棋格、方向、字符或列表元素。
            { // 开始上方函数、条件、循环或类型的作用域。
                if (adapter->Index != bestIndex) continue; // 检查 `if (adapter->Index != bestIndex) continue`；条件成立时执行括号之后或下一行的处理。
                for (IP_ADDR_STRING* ip = &adapter->IpAddressList; ip; ip = ip->Next) // 循环推进：`IP_ADDR_STRING* ip = &adapter->IpAddressList; ip; ip = ip->Next)`；逐项处理棋格、方向、字符或列表元素。
                { // 开始上方函数、条件、循环或类型的作用域。
                    auto found = std::find(hostAddresses_.begin(), hostAddresses_.end(), ip->IpAddress.String); // 声明 found：找到的地址迭代器；按右侧表达式初始化。
                    if (found != hostAddresses_.end()) // 检查 `if (found != hostAddresses_.end())`；条件成立时执行括号之后或下一行的处理。
                    { // 开始上方函数、条件、循环或类型的作用域。
                        std::rotate(hostAddresses_.begin(), found, found + 1); // 执行 `std::rotate(hostAddresses_.begin(), found, found + 1)`；调用相应对象的方法完成本步骤。
                        break; // 跳出当前循环，继续执行循环之后的代码。
                    }
                }
                break; // 跳出当前循环，继续执行循环之后的代码。
            }
        }
    }
    return true; // 返回 true：本次条件成立或操作成功；具体含义见当前函数说明。
}
bool NetworkConnection::join(const std::string& ipv4, unsigned short port) // 函数入口：网络层解析 IPv4 并非阻塞连接；联机业务层使用输入框 IP 加入房间。
{ // 开始上方函数、条件、循环或类型的作用域。
    close(); // 关闭监听和对端套接字，清理收发缓存及网络状态。
    sockaddr_in address; // 声明 address：IPv4 地址结构或地址文字；参数或长度由本行给出。
    ZeroMemory(&address, sizeof(address)); // 将 Windows 结构清零；参数为 `&address, sizeof(address))`。
    address.sin_family = AF_INET; // 更新数据：`address.sin_family = AF_INET`；赋值后的状态供后续逻辑或绘图使用。
    address.sin_port = htons(port); // 更新数据：`address.sin_port = htons(port)`；赋值后的状态供后续逻辑或绘图使用。
    // 只接受四段十进制 IPv4，不解析域名，避免阻塞主循环。
    unsigned long ip = 0; // 声明 ip：IPv4 数值或显示文字；按右侧表达式初始化。
    int part = 0, digits = 0, groups = 0; // 声明 part：当前 IPv4 十进制段值；按右侧表达式初始化。
    for (std::size_t i = 0; i <= ipv4.size(); ++i) // 循环推进：`std::size_t i = 0; i <= ipv4.size(); ++i)`；逐项处理棋格、方向、字符或列表元素。
    { // 开始上方函数、条件、循环或类型的作用域。
        const char ch = i == ipv4.size() ? '.' : ipv4[i]; // 声明 ch：当前字符；按右侧表达式初始化。
        if (ch >= '0' && ch <= '9') { part = part * 10 + ch - '0'; ++digits; } // 检查 `if (ch >= '0' && ch <= '9') { part = part * 10 + ch - '0'; ++digits; }`；条件成立时执行括号之后或下一行的处理。
        else if (ch == '.' && digits > 0 && digits <= 3 && part <= 255 && groups < 4) // 前一条件不满足时，再检查 `(ch == '.' && digits > 0 && digits <= 3 && part <= 255 && groups < 4)`。
        { ip = (ip << 8) | part; ++groups; part = digits = 0; } // 更新数据：`{ ip = (ip << 8) | part; ++groups; part = digits = 0; }`；赋值后的状态供后续逻辑或绘图使用。
        else { reject("Invalid IPv4 address"); return false; } // 拒绝连接并记录这里的错误原因，随后提前返回，防止继续使用异常连接。
        if (digits > 3) { reject("Invalid IPv4 address"); return false; } // 检查 `if (digits > 3) { reject("Invalid IPv4 address"); return false; }`；条件成立时执行括号之后或下一行的处理。
    }
    if (groups != 4 || ip == 0 || ip == 0xffffffffUL) // 检查 `if (groups != 4 || ip == 0 || ip == 0xffffffffUL)`；条件成立时执行括号之后或下一行的处理。
    { reject("Invalid IPv4 address"); return false; } // 拒绝连接并记录这里的错误原因，随后提前返回，防止继续使用异常连接。
    address.sin_addr.s_addr = htonl(ip); // 更新数据：`address.sin_addr.s_addr = htonl(ip)`；赋值后的状态供后续逻辑或绘图使用。
    if (!makeSocket(peer_)) return false; // 检查 `if (!makeSocket(peer_)) return false`；条件成立时执行括号之后或下一行的处理。
    started_ = std::chrono::steady_clock::now(); // 更新数据：`started_ = std::chrono::steady_clock::now()`；赋值后的状态供后续逻辑或绘图使用。
    const int result = connect(peer_, reinterpret_cast<sockaddr*>(&address), sizeof(address)); // 声明 result：本次操作结果；按右侧表达式初始化。
    if (result == 0) handshake(); // 检查 `if (result == 0) handshake()`；条件成立时执行括号之后或下一行的处理。
    else if (WSAGetLastError() == WSAEWOULDBLOCK) state_ = NetworkState::CONNECTING; // 前一条件不满足时，再检查 `(WSAGetLastError() == WSAEWOULDBLOCK) state_ = NetworkState::CONNECTING`。
    else { reject("Connection failed"); return false; } // 拒绝连接并记录这里的错误原因，随后提前返回，防止继续使用异常连接。
    return true; // 返回 true：本次条件成立或操作成功；具体含义见当前函数说明。
}
void NetworkConnection::handshake() // 函数入口：进入握手状态并把六字节协议问候包放入发送缓冲。
{ // 开始上方函数、条件、循环或类型的作用域。
    state_ = NetworkState::HANDSHAKING; // 更新数据：`state_ = NetworkState::HANDSHAKING`；赋值后的状态供后续逻辑或绘图使用。
    started_ = std::chrono::steady_clock::now(); // 更新数据：`started_ = std::chrono::steady_clock::now()`；赋值后的状态供后续逻辑或绘图使用。
    // 每包固定 6 字节：版本、类型、行、列、步号高字节、步号低字节。
    const char greeting[6] = {2, 0, BOARD_SIZE, 0, 0, 0}; // 握手包字段依次为协议版本2、类型0、棋盘边长13和三个零。
    outgoing_.append(greeting, 6); // 将字节追加到收发缓冲：`outgoing_.append(greeting, 6)`；后续按完整消息长度处理。
}
bool NetworkConnection::sendMove(int row, int col, int sequence) // 函数入口：校验坐标和步号，将六字节棋步消息加入发送队列，不代表对方已确认。
{ // 开始上方函数、条件、循环或类型的作用域。
    if (state_ != NetworkState::CONNECTED || row < 0 || row >= BOARD_SIZE || // 检查 `if (state_ != NetworkState::CONNECTED || row < 0 || row >= BOARD_SIZE ||`；条件成立时执行括号之后或下一行的处理。
        col < 0 || col >= BOARD_SIZE || sequence < 0 || sequence >= BOARD_SIZE * BOARD_SIZE || // 继续上方条件：`col < 0 || col >= BOARD_SIZE || sequence < 0 || sequence >= BOARD_SIZE * BOARD_SIZE ||`；&& 要求同时满足，|| 表示任一成立。
        outgoing_.size() >= 1024) return false; // 执行 `outgoing_.size() >= 1024) return false`；调用相应对象的方法完成本步骤。
    const char packet[6] = {2, 1, static_cast<char>(row), static_cast<char>(col), // 声明 packet：固定六字节协议消息；按右侧表达式初始化。
        static_cast<char>(sequence >> 8), static_cast<char>(sequence & 255)}; // 执行 `static_cast<char>(sequence >> 8), static_cast<char>(sequence & 255)}`；调用相应对象的方法完成本步骤。
    outgoing_.append(packet, 6); // 将字节追加到收发缓冲：`outgoing_.append(packet, 6)`；后续按完整消息长度处理。
    return true; // 返回 true：本次条件成立或操作成功；具体含义见当前函数说明。
}
bool NetworkConnection::sendColor(int piece, bool assignment) // 函数入口：将选色请求或房主确认加入发送队列，类型分别为 2 和 3。
{ // 开始上方函数、条件、循环或类型的作用域。
    if (state_ != NetworkState::CONNECTED || (piece != BLACK_PIECE && piece != WHITE_PIECE) || // 检查 `if (state_ != NetworkState::CONNECTED || (piece != BLACK_PIECE && piece != WHITE_PIECE) ||`；条件成立时执行括号之后或下一行的处理。
        outgoing_.size() >= 1024) return false; // 执行 `outgoing_.size() >= 1024) return false`；调用相应对象的方法完成本步骤。
    const char packet[6] = {2, static_cast<char>(assignment ? 3 : 2), // 声明 packet：固定六字节协议消息；按右侧表达式初始化。
                           static_cast<char>(piece), 0, 0, 0}; // 执行 `static_cast<char>(piece), 0, 0, 0}`；调用相应对象的方法完成本步骤。
    outgoing_.append(packet, 6); // 将字节追加到收发缓冲：`outgoing_.append(packet, 6)`；后续按完整消息长度处理。
    return true; // 返回 true：本次条件成立或操作成功；具体含义见当前函数说明。
}
bool NetworkConnection::receiveMove(NetworkMove& move) // 函数入口：按收到顺序取出一条已解析事件，兼容棋步与选色事件。
{ // 开始上方函数、条件、循环或类型的作用域。
    if (moves_.empty()) return false; // 检查 `if (moves_.empty()) return false`；条件成立时执行括号之后或下一行的处理。
    move = moves_.front(); moves_.pop_front(); return true; // 取出队头事件并删除，保证选色和落子按网络收到的顺序处理。
}
void NetworkConnection::poll() // 函数入口：每帧推进连接、发送未发完数据、接收及拆出完整六字节消息。
{ // 开始上方函数、条件、循环或类型的作用域。
    if (state_ == NetworkState::LISTENING) // 检查 `if (state_ == NetworkState::LISTENING)`；条件成立时执行括号之后或下一行的处理。
    { // 开始上方函数、条件、循环或类型的作用域。
        peer_ = accept(listener_, NULL, NULL); // 更新数据：`peer_ = accept(listener_, NULL, NULL)`；赋值后的状态供后续逻辑或绘图使用。
        if (peer_ == INVALID_SOCKET) // 检查 `if (peer_ == INVALID_SOCKET)`；条件成立时执行括号之后或下一行的处理。
        { if (WSAGetLastError() != WSAEWOULDBLOCK) reject("Accept failed"); return; } // 暂时没有数据或连接请求时仅返回；其他套接字错误会关闭连接并标记失败。
        u_long nonblocking = 1; // 声明 nonblocking：启用非阻塞模式的标志；按右侧表达式初始化。
        if (ioctlsocket(peer_, FIONBIO, &nonblocking) != 0) // 检查 `if (ioctlsocket(peer_, FIONBIO, &nonblocking) != 0)`；条件成立时执行括号之后或下一行的处理。
        { reject("Cannot configure peer socket"); return; } // 拒绝连接并记录这里的错误原因，随后提前返回，防止继续使用异常连接。
        closesocket(listener_); listener_ = INVALID_SOCKET; // 关闭 TCP 套接字；参数为 `listener_); listener_ = INVALID_SOCKET`。
        handshake(); // 进入握手状态并把六字节协议问候包放入发送缓冲。
    }
    if (state_ == NetworkState::CONNECTING || state_ == NetworkState::HANDSHAKING) // 检查 `if (state_ == NetworkState::CONNECTING || state_ == NetworkState::HANDSHAKING)`；条件成立时执行括号之后或下一行的处理。
    { // 开始上方函数、条件、循环或类型的作用域。
        if (std::chrono::steady_clock::now() - started_ > std::chrono::seconds(10)) // 检查 `if (std::chrono::steady_clock::now() - started_ > std::chrono::seconds(10))`；条件成立时执行括号之后或下一行的处理。
        { reject("Connection timed out"); return; } // 拒绝连接并记录这里的错误原因，随后提前返回，防止继续使用异常连接。
    }
    if (state_ == NetworkState::CONNECTING) // 检查 `if (state_ == NetworkState::CONNECTING)`；条件成立时执行括号之后或下一行的处理。
    { // 开始上方函数、条件、循环或类型的作用域。
        fd_set writable, failed; // 声明 writable：本行使用的局部数据；参数或长度由本行给出。
        FD_ZERO(&writable); FD_ZERO(&failed); // 清空 select 的套接字集合；参数为 `&writable); FD_ZERO(&failed)`。
        FD_SET(peer_, &writable); FD_SET(peer_, &failed); // 将对端套接字加入查询集合；参数为 `peer_, &writable); FD_SET(peer_, &failed)`。
        timeval timeout = {0, 0}; // 声明 timeout：零等待的连接状态查询超时；按右侧表达式初始化。
        if (select(0, NULL, &writable, &failed, &timeout) == SOCKET_ERROR) // 检查 `if (select(0, NULL, &writable, &failed, &timeout) == SOCKET_ERROR)`；条件成立时执行括号之后或下一行的处理。
        { reject("Connection check failed"); return; } // 拒绝连接并记录这里的错误原因，随后提前返回，防止继续使用异常连接。
        if (!FD_ISSET(peer_, &writable) && !FD_ISSET(peer_, &failed)) return; // 检查 `if (!FD_ISSET(peer_, &writable) && !FD_ISSET(peer_, &failed)) return`；条件成立时执行括号之后或下一行的处理。
        int error = 0, size = sizeof(error); // 声明 error：错误原因；按右侧表达式初始化。
        if (getsockopt(peer_, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &size) != 0 || error) // 检查 `if (getsockopt(peer_, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &size) != 0 || error)`；条件成立时执行括号之后或下一行的处理。
        { reject("Connection refused or unreachable"); return; } // 拒绝连接并记录这里的错误原因，随后提前返回，防止继续使用异常连接。
        handshake(); // 进入握手状态并把六字节协议问候包放入发送缓冲。
    }
    if (state_ != NetworkState::HANDSHAKING && state_ != NetworkState::CONNECTED) return; // 检查 `if (state_ != NetworkState::HANDSHAKING && state_ != NetworkState::CONNECTED) return`；条件成立时执行括号之后或下一行的处理。
    if (!outgoing_.empty()) // 检查 `if (!outgoing_.empty())`；条件成立时执行括号之后或下一行的处理。
    { // 开始上方函数、条件、循环或类型的作用域。
        const int sent = send(peer_, outgoing_.data(), static_cast<int>(outgoing_.size()), 0); // 声明 sent：此次成功发送的字节数；按右侧表达式初始化。
        if (sent > 0) outgoing_.erase(0, sent); // 只移除本次实际发送成功的字节，TCP 部分发送时保留剩余数据。
        else if (sent == 0 || WSAGetLastError() != WSAEWOULDBLOCK) // 前一条件不满足时，再检查 `(sent == 0 || WSAGetLastError() != WSAEWOULDBLOCK)`。
        { reject("Send failed"); return; } // 拒绝连接并记录这里的错误原因，随后提前返回，防止继续使用异常连接。
    }
    char buffer[256]; // 声明 buffer：临时接收数据或查询网卡的缓冲；参数或长度由本行给出。
    const int received = recv(peer_, buffer, sizeof(buffer), 0); // 声明 received：实际收到或读取的数据长度；按右侧表达式初始化。
    if (received == 0) { close(); state_ = NetworkState::DISCONNECTED; return; } // 检查 `if (received == 0) { close(); state_ = NetworkState::DISCONNECTED; return; }`；条件成立时执行括号之后或下一行的处理。
    if (received == SOCKET_ERROR) // 检查 `if (received == SOCKET_ERROR)`；条件成立时执行括号之后或下一行的处理。
    { if (WSAGetLastError() != WSAEWOULDBLOCK) reject("Receive failed"); return; } // 暂时没有数据或连接请求时仅返回；其他套接字错误会关闭连接并标记失败。
    incoming_.append(buffer, received); // 把此次收到的字节追加到缓存，不假设一次 recv 就对应一整条消息。
    while (incoming_.size() >= 6) // 只要 `incoming_.size() >= 6)` 成立就继续处理；用于事件、连子或网络队列。
    { // 开始上方函数、条件、循环或类型的作用域。
        const unsigned char* p = reinterpret_cast<const unsigned char*>(incoming_.data()); // 声明 p：当前六字节协议包的无符号字节指针；按右侧表达式初始化。
        if (p[0] != 2) { reject("Protocol version mismatch"); return; } // 检查 `if (p[0] != 2) { reject("Protocol version mismatch"); return; }`；条件成立时执行括号之后或下一行的处理。
        if (state_ == NetworkState::HANDSHAKING) // 检查 `if (state_ == NetworkState::HANDSHAKING)`；条件成立时执行括号之后或下一行的处理。
        { // 开始上方函数、条件、循环或类型的作用域。
            if (p[1] != 0 || p[2] != BOARD_SIZE || p[3] || p[4] || p[5]) // 检查 `if (p[1] != 0 || p[2] != BOARD_SIZE || p[3] || p[4] || p[5])`；条件成立时执行括号之后或下一行的处理。
            { reject("Invalid handshake"); return; } // 拒绝连接并记录这里的错误原因，随后提前返回，防止继续使用异常连接。
            state_ = NetworkState::CONNECTED; // 更新数据：`state_ = NetworkState::CONNECTED`；赋值后的状态供后续逻辑或绘图使用。
        }
        else // 前面的条件不满足时，执行下面的备用分支。
        { // 开始上方函数、条件、循环或类型的作用域。
            const int sequence = p[4] * 256 + p[5]; // 用步号高字节与低字节恢复大端序整数，供联机业务校验是否漏步或乱序。
            const bool validMove = p[1] == 1 && p[2] < BOARD_SIZE && p[3] < BOARD_SIZE && // 声明 validMove：是否为格式正确的棋步包；按右侧表达式初始化。
                                   sequence < BOARD_SIZE * BOARD_SIZE; // 承接上方表达式的参数、条件或初值：`sequence < BOARD_SIZE * BOARD_SIZE`。
            const bool validColor = (p[1] == 2 || p[1] == 3) && // 声明 validColor：是否为格式正确的选色包；按右侧表达式初始化。
                (p[2] == BLACK_PIECE || p[2] == WHITE_PIECE) && !p[3] && !p[4] && !p[5]; // 执行 `(p[2] == BLACK_PIECE || p[2] == WHITE_PIECE) && !p[3] && !p[4] && !p[5]`；调用相应对象的方法完成本步骤。
            if ((!validMove && !validColor) || moves_.size() >= 256) // 检查 `if ((!validMove && !validColor) || moves_.size() >= 256)`；条件成立时执行括号之后或下一行的处理。
            { reject("Invalid move packet"); return; } // 拒绝连接并记录这里的错误原因，随后提前返回，防止继续使用异常连接。
            moves_.push_back(NetworkMove{p[2], p[3], sequence, p[1]}); // 追加一项到 `moves_`，保留数据的先后顺序。
        }
        incoming_.erase(0, 6); // 移除刚处理的六字节消息，后面的消息继续解析，不完整尾部留待下次接收。
    }
}
