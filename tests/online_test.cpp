// 文件职责：自动化验证：断言规则、回合、文件或网络行为符合预期；不参与游戏主程序。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#include <winsock2.h> // 引入 <winsock2.h>，提供本文件使用的类型和函数声明。
#include "../board.h"
#include "../modes/online/online.h" // 引入 "../modes/online/online.h"，提供本文件使用的类型和函数声明。
#include <cassert> // 引入 <cassert>，提供本文件使用的类型和函数声明。
#include <iostream> // 引入 <iostream>，提供本文件使用的类型和函数声明。

template<class Predicate> // 定义泛型测试辅助函数，让等待条件可以是不同的 lambda 表达式。
void waitFor(OnlineSession& host, OnlineSession& guest, Predicate done) // 函数入口：轮询两个测试会话，最多等三秒，断言预期条件已经成立。
{ // 开始上方函数、条件、循环或类型的作用域。
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3); // 声明 deadline：测试等待的截止时刻；按右侧表达式初始化。
    while (!done() && std::chrono::steady_clock::now() < deadline) // 只要 `!done() && std::chrono::steady_clock::now() < deadline)` 成立就继续处理；用于事件、连子或网络队列。
    { host.update(); guest.update(); Sleep(1); } // 承接上方表达式的参数、条件或初值：`{ host.update(); guest.update(); Sleep(1); }`。
    assert(done()); // 测试断言：要求 `done(` 成立；失败立即终止测试。
}
int main() // 独立测试入口：验证真实 TCP、IP 编辑、选色、同步、断线与拆包粘包；不链接进游戏主程序。
{ // 开始上方函数、条件、循环或类型的作用域。
    OnlineSession host, guest; // 声明 host：测试房主会话；参数或长度由本行给出。
    assert(guest.serverIp.empty()); // 测试断言：要求 `guest.serverIp.empty(` 成立；失败立即终止测试。
    // 主循环使用的界面入口：输入 IP、创建、加入、落子和返回。
    assert(!onlineClick(guest, 250, 300)); // 测试断言：要求 `!onlineClick(guest, 250, 300` 成立；失败立即终止测试。
    assert(guest.editingIp); // 测试断言：要求 `guest.editingIp` 成立；失败立即终止测试。
    for (int i = 0; i < 9; ++i) onlineKey(guest, 8); // 循环推进：`int i = 0; i < 9; ++i) onlineKey(guest, 8)`；逐项处理棋格、方向、字符或列表元素。
    const std::string loopback = "127.0.0.1"; // 声明 loopback：本行使用的局部数据；按右侧表达式初始化。
    for (char ch : loopback) onlineKey(guest, ch); // 循环推进：`char ch : loopback) onlineKey(guest, ch)`；逐项处理棋格、方向、字符或列表元素。
    onlineKey(guest, 'x'); // 编辑 IPv4 输入框光标及数字点号，Enter 发起加入。
    assert(guest.serverIp == loopback); // 测试断言：要求 `guest.serverIp == loopback` 成立；失败立即终止测试。
    onlineKey(guest, 256 + 71); // 编辑 IPv4 输入框光标及数字点号，Enter 发起加入。
    onlineKey(guest, '2'); // 编辑 IPv4 输入框光标及数字点号，Enter 发起加入。
    assert(guest.serverIp == "2127.0.0.1" && guest.ipCursor == 1); // 测试断言：要求 `guest.serverIp == "2127.0.0.1" && guest.ipCursor == 1` 成立；失败立即终止测试。
    onlineKey(guest, 8); // 编辑 IPv4 输入框光标及数字点号，Enter 发起加入。
    assert(guest.serverIp == loopback && guest.ipCursor == 0); // 测试断言：要求 `guest.serverIp == loopback && guest.ipCursor == 0` 成立；失败立即终止测试。
    onlineKey(guest, 256 + 79); // 编辑 IPv4 输入框光标及数字点号，Enter 发起加入。
    assert(!onlineClick(host, 300, 220)); // 测试断言：要求 `!onlineClick(host, 300, 220` 成立；失败立即终止测试。
    if (host.network.state() != NetworkState::LISTENING) std::cerr << host.network.error() << std::endl;
    assert(host.network.state() == NetworkState::LISTENING); // 测试断言：要求 `host.network.state() == NetworkState::LISTENING` 成立；失败立即终止测试。
    onlineKey(guest, 13); // 编辑 IPv4 输入框光标及数字点号，Enter 发起加入。
    waitFor(host, guest, [&] { return host.network.state() == NetworkState::CONNECTED && // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
                                    guest.network.state() == NetworkState::CONNECTED; }); // 执行 `guest.network.state() == NetworkState::CONNECTED; })`；调用相应对象的方法完成本步骤。
    assert(host.localPiece == EMPTY && guest.localPiece == EMPTY); // 测试断言：要求 `host.localPiece == EMPTY && guest.localPiece == EMPTY` 成立；失败立即终止测试。
    assert(!host.move(0, 0) && !guest.move(0, 0)); // 测试断言：要求 `!host.move(0, 0) && !guest.move(0, 0` 成立；失败立即终止测试。
    assert(!onlineClick(guest, 200, 430)); // 测试断言：要求 `!onlineClick(guest, 200, 430` 成立；失败立即终止测试。
    waitFor(host, guest, [&] { return host.localPiece == WHITE_PIECE && guest.localPiece == BLACK_PIECE; }); // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
    assert(!onlineClick(guest, BOARD_LEFT, BOARD_TOP)); // 测试断言：要求 `!onlineClick(guest, BOARD_LEFT, BOARD_TOP` 成立；失败立即终止测试。
    waitFor(host, guest, [&] { return host.game.moves.size() == 1; }); // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
    assert(host.game.board[0][0] == BLACK_PIECE); // 测试断言：要求 `host.game.board[0][0] == BLACK_PIECE` 成立；失败立即终止测试。
    assert(onlineClick(guest, 600, 470)); // 测试断言：要求 `onlineClick(guest, 600, 470` 成立；失败立即终止测试。
    waitFor(host, guest, [&] { return host.network.state() == NetworkState::DISCONNECTED; }); // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
    host.reset(); // 执行 `host.reset()`；调用相应对象的方法完成本步骤。
    assert(host.create()); assert(guest.join()); // 测试断言：要求 `host.create()); assert(guest.join(` 成立；失败立即终止测试。
    waitFor(host, guest, [&] { return host.network.state() == NetworkState::CONNECTED && // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
                                    guest.network.state() == NetworkState::CONNECTED; }); // 执行 `guest.network.state() == NetworkState::CONNECTED; })`；调用相应对象的方法完成本步骤。
    // 双方同时抢黑，房主确认结果保持两端相反。
    assert(host.chooseColor(BLACK_PIECE)); // 测试断言：要求 `host.chooseColor(BLACK_PIECE` 成立；失败立即终止测试。
    assert(guest.chooseColor(BLACK_PIECE)); // 测试断言：要求 `guest.chooseColor(BLACK_PIECE` 成立；失败立即终止测试。
    waitFor(host, guest, [&] { return guest.localPiece == WHITE_PIECE; }); // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
    assert(host.localPiece == BLACK_PIECE && !guest.chooseColor(BLACK_PIECE)); // 测试断言：要求 `host.localPiece == BLACK_PIECE && !guest.chooseColor(BLACK_PIECE` 成立；失败立即终止测试。
    assert(!onlineClick(host, BOARD_LEFT, BOARD_TOP)); // 测试断言：要求 `!onlineClick(host, BOARD_LEFT, BOARD_TOP` 成立；失败立即终止测试。
    waitFor(host, guest, [&] { return guest.game.moves.size() == 1; }); // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
    assert(guest.game.board[0][0] == BLACK_PIECE); // 测试断言：要求 `guest.game.board[0][0] == BLACK_PIECE` 成立；失败立即终止测试。
    assert(onlineClick(guest, 600, 470)); // 测试断言：要求 `onlineClick(guest, 600, 470` 成立；失败立即终止测试。
    assert(guest.localPiece == EMPTY && guest.game.moves.empty()); // 测试断言：要求 `guest.localPiece == EMPTY && guest.game.moves.empty(` 成立；失败立即终止测试。
    waitFor(host, guest, [&] { return host.network.state() == NetworkState::DISCONNECTED; }); // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
    host.reset(); // 执行 `host.reset()`；调用相应对象的方法完成本步骤。
    const unsigned short port = 18888; // 声明 port：TCP 端口号；按右侧表达式初始化。
    assert(host.create(port)); // 测试断言：要求 `host.create(port` 成立；失败立即终止测试。
    assert(guest.join(port)); // 测试断言：要求 `guest.join(port` 成立；失败立即终止测试。
    waitFor(host, guest, [&] { return host.network.state() == NetworkState::CONNECTED && // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
                                    guest.network.state() == NetworkState::CONNECTED; }); // 执行 `guest.network.state() == NetworkState::CONNECTED; })`；调用相应对象的方法完成本步骤。
    assert(!guest.move(1, 1)); // 白棋不能抢先落子。
    assert(host.chooseColor(BLACK_PIECE)); // 测试断言：要求 `host.chooseColor(BLACK_PIECE` 成立；失败立即终止测试。
    waitFor(host, guest, [&] { return guest.localPiece == WHITE_PIECE; }); // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
    for (int c = 0; c < 5; ++c) // 循环推进：`int c = 0; c < 5; ++c)`；逐项处理棋格、方向、字符或列表元素。
    { // 开始上方函数、条件、循环或类型的作用域。
        assert(host.move(0, c)); // 测试断言：要求 `host.move(0, c` 成立；失败立即终止测试。
        assert(!host.move(0, c)); // 测试断言：要求 `!host.move(0, c` 成立；失败立即终止测试。
        waitFor(host, guest, [&] { return guest.game.moves.size() == host.game.moves.size(); }); // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
        if (c < 4) // 检查 `if (c < 4)`；条件成立时执行括号之后或下一行的处理。
        { // 开始上方函数、条件、循环或类型的作用域。
            assert(guest.move(2, c)); // 测试断言：要求 `guest.move(2, c` 成立；失败立即终止测试。
            waitFor(host, guest, [&] { return host.game.moves.size() == guest.game.moves.size(); }); // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
        }
    }
    assert(host.game.winner == BLACK_PIECE && guest.game.winner == BLACK_PIECE); // 测试断言：要求 `host.game.winner == BLACK_PIECE && guest.game.winner == BLACK_PIECE` 成立；失败立即终止测试。
    for (int r = 0; r < BOARD_SIZE; ++r) // 循环推进：`int r = 0; r < BOARD_SIZE; ++r)`；逐项处理棋格、方向、字符或列表元素。
        for (int c = 0; c < BOARD_SIZE; ++c) // 循环推进：`int c = 0; c < BOARD_SIZE; ++c)`；逐项处理棋格、方向、字符或列表元素。
            assert(host.game.board[r][c] == guest.game.board[r][c]); // 测试断言：要求 `host.game.board[r][c] == guest.game.board[r][c]` 成立；失败立即终止测试。
    assert(!guest.move(3, 3)); // 测试断言：要求 `!guest.move(3, 3` 成立；失败立即终止测试。
    guest.reset(); // 执行 `guest.reset()`；调用相应对象的方法完成本步骤。
    waitFor(host, guest, [&] { return host.network.state() == NetworkState::DISCONNECTED; }); // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
    host.reset(); // 执行 `host.reset()`；调用相应对象的方法完成本步骤。
    guest.serverIp = "999.1.2.3"; // 更新数据：`guest.serverIp = "999.1.2.3"`；赋值后的状态供后续逻辑或绘图使用。
    assert(!guest.join(port)); // 测试断言：要求 `!guest.join(port` 成立；失败立即终止测试。
    assert(guest.network.state() == NetworkState::FAILED); // 测试断言：要求 `guest.network.state() == NetworkState::FAILED` 成立；失败立即终止测试。
    assert(!guest.inRoom && !guest.chooseColor(BLACK_PIECE)); // 测试断言：要求 `!guest.inRoom && !guest.chooseColor(BLACK_PIECE` 成立；失败立即终止测试。
    guest.reset(); guest.serverIp = "127.0.0.1"; // 更新数据：`guest.reset(); guest.serverIp = "127.0.0.1"`；赋值后的状态供后续逻辑或绘图使用。
    // No server is listening: an asynchronous failure must allow retrying.
    guest.join(port); // 执行 `guest.join(port)`；调用相应对象的方法完成本步骤。
    waitFor(host, guest, [&] { return guest.network.state() == NetworkState::FAILED; }); // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
    assert(!guest.inRoom && !guest.chooseColor(BLACK_PIECE)); // 测试断言：要求 `!guest.inRoom && !guest.chooseColor(BLACK_PIECE` 成立；失败立即终止测试。
    guest.reset(); guest.serverIp = "127.0.0.1"; // 更新数据：`guest.reset(); guest.serverIp = "127.0.0.1"`；赋值后的状态供后续逻辑或绘图使用。
    assert(host.create(port)); assert(guest.join(port)); // 测试断言：要求 `host.create(port)); assert(guest.join(port` 成立；失败立即终止测试。
    OnlineSession duplicateHost; // 声明 duplicateHost：本行使用的局部数据；参数或长度由本行给出。
    assert(!duplicateHost.create(port)); // 测试断言：要求 `!duplicateHost.create(port` 成立；失败立即终止测试。
    assert(duplicateHost.network.error().find("10048") != std::string::npos); // 端口冲突必须保留明确的系统错误码。
    assert(!duplicateHost.inRoom); // 测试断言：要求 `!duplicateHost.inRoom` 成立；失败立即终止测试。
    waitFor(host, guest, [&] { return host.network.state() == NetworkState::CONNECTED && // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
                                    guest.network.state() == NetworkState::CONNECTED; }); // 执行 `guest.network.state() == NetworkState::CONNECTED; })`；调用相应对象的方法完成本步骤。
    assert(host.inRoom && guest.inRoom); // 测试断言：要求 `host.inRoom && guest.inRoom` 成立；失败立即终止测试。
    assert(guest.network.sendMove(0, 0, 0)); // 非法抢回合消息应拒绝。
    waitFor(host, guest, [&] { return host.network.state() == NetworkState::FAILED; }); // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
    assert(host.game.moves.empty()); // 测试断言：要求 `host.game.moves.empty(` 成立；失败立即终止测试。
    host.reset(); guest.reset(); // 执行 `host.reset(); guest.reset()`；调用相应对象的方法完成本步骤。

    // Use the displayed host IP even when both sessions run on this computer.
    assert(host.create(port)); // 测试断言：要求 `host.create(port` 成立；失败立即终止测试。
    if (!host.network.hostAddresses().empty()) // 检查 `if (!host.network.hostAddresses().empty())`；条件成立时执行括号之后或下一行的处理。
    { // 开始上方函数、条件、循环或类型的作用域。
        guest.serverIp = host.network.hostAddresses().front(); // 更新数据：`guest.serverIp = host.network.hostAddresses().front()`；赋值后的状态供后续逻辑或绘图使用。
        std::cout << "Connecting via displayed host IP: " << guest.serverIp << "\n"; // 输出测试执行结果，便于确认测试是否运行到末尾。
        assert(guest.join(port)); // 测试断言：要求 `guest.join(port` 成立；失败立即终止测试。
        waitFor(host, guest, [&] { return host.network.state() == NetworkState::CONNECTED && // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
                                        guest.network.state() == NetworkState::CONNECTED; }); // 执行 `guest.network.state() == NetworkState::CONNECTED; })`；调用相应对象的方法完成本步骤。
        assert(host.chooseColor(BLACK_PIECE)); // 测试断言：要求 `host.chooseColor(BLACK_PIECE` 成立；失败立即终止测试。
        waitFor(host, guest, [&] { return guest.localPiece == WHITE_PIECE; }); // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
        assert(host.move(1, 1)); // 测试断言：要求 `host.move(1, 1` 成立；失败立即终止测试。
        waitFor(host, guest, [&] { return guest.game.board[1][1] == BLACK_PIECE; }); // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
    }
    host.reset(); guest.reset(); // 执行 `host.reset(); guest.reset()`；调用相应对象的方法完成本步骤。

    // 用原始 socket 分段发送握手、合并发送棋步，验证 TCP 拆包与粘包。
    assert(host.create(port)); // 测试断言：要求 `host.create(port` 成立；失败立即终止测试。
    SOCKET raw = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 声明 raw：测试使用的原始 TCP 套接字；按右侧表达式初始化。
    sockaddr_in address; // 声明 address：IPv4 地址结构或地址文字；参数或长度由本行给出。
    ZeroMemory(&address, sizeof(address)); // 将 Windows 结构清零；参数为 `&address, sizeof(address))`。
    address.sin_family = AF_INET; address.sin_port = htons(port); // 更新数据：`address.sin_family = AF_INET; address.sin_port = htons(port)`；赋值后的状态供后续逻辑或绘图使用。
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 更新数据：`address.sin_addr.s_addr = htonl(INADDR_LOOPBACK)`；赋值后的状态供后续逻辑或绘图使用。
    assert(connect(raw, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == 0); // 测试断言：要求 `connect(raw, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == 0` 成立；失败立即终止测试。
    host.update(); // 执行 `host.update()`；调用相应对象的方法完成本步骤。
    const char hello[6] = {2, 0, BOARD_SIZE, 0, 0, 0}; // 声明 hello：测试构造的握手数据；按右侧表达式初始化。
    assert(send(raw, hello, 2, 0) == 2); host.update(); // 测试断言：要求 `send(raw, hello, 2, 0) == 2); host.update(` 成立；失败立即终止测试。
    assert(host.network.state() == NetworkState::HANDSHAKING); // 测试断言：要求 `host.network.state() == NetworkState::HANDSHAKING` 成立；失败立即终止测试。
    assert(send(raw, hello + 2, 4, 0) == 4); // 测试断言：要求 `send(raw, hello + 2, 4, 0) == 4` 成立；失败立即终止测试。
    waitFor(host, guest, [&] { return host.network.state() == NetworkState::CONNECTED; }); // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
    assert(host.chooseColor(BLACK_PIECE)); // 测试断言：要求 `host.chooseColor(BLACK_PIECE` 成立；失败立即终止测试。
    assert(host.move(0, 0)); // 测试断言：要求 `host.move(0, 0` 成立；失败立即终止测试。
    // 白棋合法一步后又抢走一步；第一步应用，第二步拒绝。
    const char packets[12] = {2, 1, 2, 0, 0, 1, 2, 1, 2, 1, 0, 2}; // 声明 packets：测试合并发送的两条棋步消息；按右侧表达式初始化。
    assert(send(raw, packets, 12, 0) == 12); // 测试断言：要求 `send(raw, packets, 12, 0) == 12` 成立；失败立即终止测试。
    waitFor(host, guest, [&] { return host.network.state() == NetworkState::FAILED; }); // 轮询两个测试会话，最多等三秒，断言预期条件已经成立。
    assert(host.game.moves.size() == 2 && host.game.board[2][1] == EMPTY); // 测试断言：要求 `host.game.moves.size() == 2 && host.game.board[2][1] == EMPTY` 成立；失败立即终止测试。
    closesocket(raw); // 关闭 TCP 套接字；参数为 `raw)`。
    std::cout << "Online loopback tests passed\n"; // 输出测试执行结果，便于确认测试是否运行到末尾。
}
