// 文件职责：联机业务及界面：房间、IP 编辑、选色和棋步验证；底层传输交给 NetworkConnection。
// 阅读提示：每个非空代码行均附中文解释；空行仅用于分段。所有注释不参与运行。
#include "online.h" // 引入 "online.h"，提供本文件使用的类型和函数声明。
#include "../../board.h"
#include "../../ui/menu.h" // 引入 "../../ui/menu.h"，提供本文件使用的类型和函数声明。
#include "../../ui/match.h" // 引入 "../../ui/match.h"，提供本文件使用的类型和函数声明。
#include <ege.h> // 引入 <ege.h>，提供本文件使用的类型和函数声明。
using namespace ege; // 允许直接使用该命名空间中的名称，省略 ege:: 或 std:: 前缀。

bool OnlineSession::create(unsigned short port) // 函数入口：重置会话、标记房主并调用网络监听。
{ // 开始上方函数、条件、循环或类型的作用域。
    reset(); isHost = true; // 关闭联机连接并清空棋局和选色状态，保留输入的服务器 IP。
    inRoom = network.host(port); // 更新数据：`inRoom = network.host(port)`；赋值后的状态供后续逻辑或绘图使用。
    return inRoom; // 返回 `inRoom`，将结果交给调用者。
}
bool OnlineSession::join(unsigned short port) // 函数入口：网络层解析 IPv4 并非阻塞连接；联机业务层使用输入框 IP 加入房间。
{ // 开始上方函数、条件、循环或类型的作用域。
    reset(); // 关闭联机连接并清空棋局和选色状态，保留输入的服务器 IP。
    inRoom = network.join(serverIp, port); // 更新数据：`inRoom = network.join(serverIp, port)`；赋值后的状态供后续逻辑或绘图使用。
    return inRoom; // 返回 `inRoom`，将结果交给调用者。
}
void OnlineSession::reset() // 函数入口：关闭联机连接并清空棋局和选色状态，保留输入的服务器 IP。
{ // 开始上方函数、条件、循环或类型的作用域。
    network.close(); game = GameState(); localPiece = EMPTY; editingIp = false; // 更新数据：`network.close(); game = GameState(); localPiece = EMPTY; editingIp = false`；赋值后的状态供后续逻辑或绘图使用。
    inRoom = false; isHost = false; choicePending = false; // 更新数据：`inRoom = false; isHost = false; choicePending = false`；赋值后的状态供后续逻辑或绘图使用。
    showOtherAddresses = false; // 更新数据：`showOtherAddresses = false`；赋值后的状态供后续逻辑或绘图使用。
    ipCursor = serverIp.size(); // 更新数据：`ipCursor = serverIp.size()`；赋值后的状态供后续逻辑或绘图使用。
}
bool OnlineSession::chooseColor(PieceColor piece) // 函数入口：房主直接确认颜色，加入方先发送请求，确认后双方自动取相反颜色。
{ // 开始上方函数、条件、循环或类型的作用域。
    if (!inRoom || network.state() != NetworkState::CONNECTED || localPiece != EMPTY || // 检查 `if (!inRoom || network.state() != NetworkState::CONNECTED || localPiece != EMPTY ||`；条件成立时执行括号之后或下一行的处理。
        choicePending || (piece != BLACK_PIECE && piece != WHITE_PIECE)) return false; // 执行 `choicePending || (piece != BLACK_PIECE && piece != WHITE_PIECE)) return false`；调用相应对象的方法完成本步骤。
    if (!network.sendColor(piece, isHost)) return false; // 检查 `if (!network.sendColor(piece, isHost)) return false`；条件成立时执行括号之后或下一行的处理。
    if (isHost) localPiece = piece; // 房主是选色结果的权威，发送确认后立即保存自己执棋颜色。
    else choicePending = true; // 加入方仅发送选色请求，要等房主确认，不能立即假设选色已成功。
    return true; // 返回 true：本次条件成立或操作成功；具体含义见当前函数说明。
}
void OnlineSession::update() // 函数入口：推进网络并按顺序处理选色与棋步，拒绝非法回合和错误步号。
{ // 开始上方函数、条件、循环或类型的作用域。
    network.poll(); // 执行 `network.poll()`；调用相应对象的方法完成本步骤。
    if (localPiece == EMPTY && (network.state() == NetworkState::FAILED || // 检查 `if (localPiece == EMPTY && (network.state() == NetworkState::FAILED ||`；条件成立时执行括号之后或下一行的处理。
                               network.state() == NetworkState::DISCONNECTED)) // 承接上方表达式的参数、条件或初值：`network.state() == NetworkState::DISCONNECTED))`。
    { // 开始上方函数、条件、循环或类型的作用域。
        inRoom = false; // 更新数据：`inRoom = false`；赋值后的状态供后续逻辑或绘图使用。
        choicePending = false; // 更新数据：`choicePending = false`；赋值后的状态供后续逻辑或绘图使用。
    }
    NetworkMove received; // 承接上方表达式的参数、条件或初值：`NetworkMove received`。
    while (network.receiveMove(received)) // 只要 `network.receiveMove(received))` 成立就继续处理；用于事件、连子或网络队列。
    { // 开始上方函数、条件、循环或类型的作用域。
        if (received.type == 2) // 检查 `if (received.type == 2)`；条件成立时执行括号之后或下一行的处理。
        { // 开始上方函数、条件、循环或类型的作用域。
            if (!isHost) { network.reject("Unexpected color request"); return; } // 检查 `if (!isHost) { network.reject("Unexpected color request"); return; }`；条件成立时执行括号之后或下一行的处理。
            // 房主统一确定颜色；同时选择时以房主已确认的结果为准。
            if (localPiece == EMPTY) // 检查 `if (localPiece == EMPTY)`；条件成立时执行括号之后或下一行的处理。
                chooseColor(received.row == BLACK_PIECE ? WHITE_PIECE : BLACK_PIECE); // 房主直接确认颜色，加入方先发送请求，确认后双方自动取相反颜色。
            continue; // 跳过本轮剩余处理，继续下一条网络事件或循环元素。
        }
        if (received.type == 3) // 检查 `if (received.type == 3)`；条件成立时执行括号之后或下一行的处理。
        { // 开始上方函数、条件、循环或类型的作用域。
            if (isHost || localPiece != EMPTY) // 检查 `if (isHost || localPiece != EMPTY)`；条件成立时执行括号之后或下一行的处理。
            { network.reject("Unexpected color assignment"); return; } // 承接上方表达式的参数、条件或初值：`{ network.reject("Unexpected color assignment"); return; }`。
            localPiece = received.row == BLACK_PIECE ? WHITE_PIECE : BLACK_PIECE; // 更新数据：`localPiece = received.row == BLACK_PIECE ? WHITE_PIECE : BLACK_PIECE`；赋值后的状态供后续逻辑或绘图使用。
            choicePending = false; // 更新数据：`choicePending = false`；赋值后的状态供后续逻辑或绘图使用。
            continue; // 跳过本轮剩余处理，继续下一条网络事件或循环元素。
        }
        if (network.state() != NetworkState::CONNECTED || localPiece == EMPTY || // 检查 `if (network.state() != NetworkState::CONNECTED || localPiece == EMPTY ||`；条件成立时执行括号之后或下一行的处理。
            game.currentPlayer == localPiece || // 继续上方条件：`game.currentPlayer == localPiece ||`；&& 要求同时满足，|| 表示任一成立。
            received.sequence != static_cast<int>(game.moves.size()) || // 继续上方条件：`received.sequence != static_cast<int>(game.moves.size()) ||`；&& 要求同时满足，|| 表示任一成立。
            !placePiece(game, received.row, received.col)) // 承接上方表达式的参数、条件或初值：`!placePiece(game, received.row, received.col))`。
        { network.reject("Illegal move or out-of-order sequence"); return; } // 承接上方表达式的参数、条件或初值：`{ network.reject("Illegal move or out-of-order sequence"); return; }`。
    }
}
bool OnlineSession::move(int row, int col) // 函数入口：仅本方回合允许落子，先加入发送队列成功后再更新棋盘。
{ // 开始上方函数、条件、循环或类型的作用域。
    if (network.state() != NetworkState::CONNECTED || localPiece == EMPTY || // 检查 `if (network.state() != NetworkState::CONNECTED || localPiece == EMPTY ||`；条件成立时执行括号之后或下一行的处理。
        game.currentPlayer != localPiece || game.winner != EMPTY || // 继续上方条件：`game.currentPlayer != localPiece || game.winner != EMPTY ||`；&& 要求同时满足，|| 表示任一成立。
        row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE || // 继续上方条件：`row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE ||`；&& 要求同时满足，|| 表示任一成立。
        game.board[row][col] != EMPTY) return false; // 承接上方表达式的参数、条件或初值：`game.board[row][col] != EMPTY) return false`。
    // 发送队列接纳后才修改棋盘；失败则不落子。
    if (!network.sendMove(row, col, static_cast<int>(game.moves.size()))) return false; // 先让发送队列接纳棋步，失败则不改变棋盘；排队成功仍不代表对方确认收到。
    return placePiece(game, row, col); // 调用共用规则实际落子，网络传输不自行实现第二套胜负判断。
}
namespace // 开启匿名命名空间，让内部辅助函数只在当前源文件可见。
{ // 开始上方函数、条件、循环或类型的作用域。
    const wchar_t* status(const OnlineSession& session) // 函数入口：将网络连接状态、选色状态及棋局状态转换为联机界面说明。
    { // 开始上方函数、条件、循环或类型的作用域。
        switch (session.network.state()) // 根据当前网络状态选择对应界面提示分支。
        { // 开始上方函数、条件、循环或类型的作用域。
        case NetworkState::LISTENING: return L"等待另一位玩家加入"; // 处理 `NetworkState::LISTENING: return L"等待另一位玩家加入"` 对应的状态。
        case NetworkState::CONNECTING: return L"正在连接服务器"; // 处理 `NetworkState::CONNECTING: return L"正在连接服务器"` 对应的状态。
        case NetworkState::HANDSHAKING: return L"正在确认对局协议"; // 处理 `NetworkState::HANDSHAKING: return L"正在确认对局协议"` 对应的状态。
        case NetworkState::DISCONNECTED: return L"对方已断开连接"; // 处理 `NetworkState::DISCONNECTED: return L"对方已断开连接"` 对应的状态。
        case NetworkState::FAILED:
            if (session.isHost && session.network.error().find("10048") != std::string::npos)
                return L"创建失败：端口已占用，请关闭其他房间或占用程序";
            if (session.isHost && session.network.error().find("10013") != std::string::npos)
                return L"创建失败：系统禁止使用此端口";
            return session.isHost ? L"创建房间失败，请查看下方原因" : L"连接失败或对局数据异常"; // 处理 `NetworkState::FAILED: return L"连接失败或对局数据异常"` 对应的状态。
        case NetworkState::CONNECTED: // 处理 `NetworkState::CONNECTED:` 对应的状态。
            if (session.localPiece == EMPTY) // 检查 `if (session.localPiece == EMPTY)`；条件成立时执行括号之后或下一行的处理。
                return session.choicePending ? L"正在确认执棋颜色" : L"双方已入房，请选择黑白棋"; // 返回 `session.choicePending ? L"正在确认执棋颜色" : L"双方已入房，请选择黑白棋"`，将结果交给调用者。
            if (session.game.winner != EMPTY) return matchStatus(session.game); // 检查 `if (session.game.winner != EMPTY) return matchStatus(session.game)`；条件成立时执行括号之后或下一行的处理。
            if (session.game.moves.size() >= BOARD_SIZE * BOARD_SIZE) return L"棋盘已满，和棋"; // 检查 `if (session.game.moves.size() >= BOARD_SIZE * BOARD_SIZE) return L"棋盘已满，和棋"`；条件成立时执行括号之后或下一行的处理。
            return session.game.currentPlayer == session.localPiece ? L"轮到你落子" : L"等待对方落子"; // 返回 `session.game.currentPlayer == session.localPiece ? L"轮到你落子" : L"等待对方落子"`，将结果交给调用者。
        default: return L"创建对局，或输入 IP 加入"; // 没有命中前面状态时，使用大厅默认提示。
        }
    }
}
void drawOnlineScreen(const OnlineSession& session, int mouseX, int mouseY) // 函数入口：根据大厅、等待房间或已选色对局状态绘制对应联机界面。
{ // 开始上方函数、条件、循环或类型的作用域。
    if (session.localPiece != EMPTY) // 检查 `if (session.localPiece != EMPTY)`；条件成立时执行括号之后或下一行的处理。
    { // 开始上方函数、条件、循环或类型的作用域。
        const bool canMove = session.network.state() == NetworkState::CONNECTED && // 声明 canMove：本行使用的局部数据；按右侧表达式初始化。
            session.game.currentPlayer == session.localPiece; // 承接上方表达式的参数、条件或初值：`session.game.currentPlayer == session.localPiece`。
        drawMatch(session.game, L"联机对决", // 绘制共用棋盘页；canMove 只控制准星，实际合法性由落子函数检查。
                  session.localPiece == BLACK_PIECE ? L"你执黑，黑棋先手" : L"你执白，等待黑棋先手", // 条件表达式：`session.localPiece == BLACK_PIECE ? L"你执黑，黑棋先手" : L"你执白，等待黑棋先手",`；根据条件选择两个值之一。
                  status(session), canMove, mouseX, mouseY); // 将网络连接状态、选色状态及棋局状态转换为联机界面说明。
        setfont(14, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `14, 0, L"微软雅黑")`。
        outtextxy(560, 275, L"联机对局 · 端口 18888"); // 在指定坐标绘制文字；参数为 `560, 275, L"联机对局 · 端口 18888")`。
        if (!session.network.error().empty()) // 检查 `if (!session.network.error().empty())`；条件成立时执行括号之后或下一行的处理。
        { // 开始上方函数、条件、循环或类型的作用域。
            const std::string& error = session.network.error(); // 声明 error：错误原因；按右侧表达式初始化。
            const std::wstring message(error.begin(), error.end()); // 声明 message：待显示的提示文字；参数或长度由本行给出。
            outtextxy(28, 550, message.c_str()); // 在指定坐标绘制文字；参数为 `28, 550, message.c_str())`。
        }
        return; // 提前结束当前无返回值函数，避免继续执行后续处理。
    }
    if (session.inRoom) // 检查 `if (session.inRoom)`；条件成立时执行括号之后或下一行的处理。
    { // 开始上方函数、条件、循环或类型的作用域。
        setfont(34, 0, L"微软雅黑"); setbkmode(TRANSPARENT); // 设置后续文字的字体和字号；参数为 `34, 0, L"微软雅黑"); setbkmode(TRANSPARENT)`。
        setcolor(EGERGB(45, 37, 26)); // 设置后续线条和文字颜色；参数为 `EGERGB(45, 37, 26))`。
        outtextxy(310, 95, L"联机房间"); // 在指定坐标绘制文字；参数为 `310, 95, L"联机房间")`。
        setfont(22, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `22, 0, L"微软雅黑")`。
        outtextxy(200, 175, status(session)); // 在指定坐标绘制文字；参数为 `200, 175, status(session))`。
        setfont(18, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `18, 0, L"微软雅黑")`。
        outtextxy(200, 225, session.isHost ? L"你是房主 · 端口 18888" : // 在指定坐标绘制文字；参数为 `200, 225, session.isHost ? L"你是房主 · 端口 18888" :`。
                  (session.network.state() == NetworkState::CONNECTED ? // 条件表达式：`(session.network.state() == NetworkState::CONNECTED ?`；根据条件选择两个值之一。
                   L"你已加入房间" : L"尚未加入房间，请等待连接确认")); // 承接上方表达式的参数、条件或初值：`L"你已加入房间" : L"尚未加入房间，请等待连接确认"))`。
        if (session.isHost) // 检查 `if (session.isHost)`；条件成立时执行括号之后或下一行的处理。
        { // 开始上方函数、条件、循环或类型的作用域。
            setfont(16, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `16, 0, L"微软雅黑")`。
            const auto& addresses = session.network.hostAddresses(); // 声明 addresses：本机可供加入的地址列表；按右侧表达式初始化。
            if (addresses.empty()) // 检查 `if (addresses.empty())`；条件成立时执行括号之后或下一行的处理。
                outtextxy(80, 260, L"未检测到可用房主 IP，请检查网络连接后重新创建"); // 在指定坐标绘制文字；参数为 `80, 260, L"未检测到可用房主 IP，请检查网络连接后重新创建")`。
            else // 前面的条件不满足时，执行下面的备用分支。
            { // 开始上方函数、条件、循环或类型的作用域。
                const std::wstring primary(addresses.front().begin(), addresses.front().end()); // 声明 primary：首选房主 IP 显示文字；参数或长度由本行给出。
                setfont(24, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `24, 0, L"微软雅黑")`。
                outtextxy(80, 260, (L"房主 IP：" + primary).c_str()); // 在指定坐标绘制文字；参数为 `80, 260, (L"房主 IP：" + primary).c_str())`。
                setfont(16, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `16, 0, L"微软雅黑")`。
                outtextxy(80, 295, L"加入方填写此 IP；双方需能通过所在网络互相连接"); // 在指定坐标绘制文字；参数为 `80, 295, L"加入方填写此 IP；双方需能通过所在网络互相连接")`。
                if (addresses.size() > 1) // 检查 `if (addresses.size() > 1)`；条件成立时执行括号之后或下一行的处理。
                    drawButton(520, 250, 200, 40, // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
                               session.showOtherAddresses ? L"收起其他地址" : L"其他地址", mouseX, mouseY); // 条件表达式：`session.showOtherAddresses ? L"收起其他地址" : L"其他地址", mouseX, mouseY)`；根据条件选择两个值之一。
                std::wstring lineText; // 声明 lineText：一行内拼接的多个 IP；参数或长度由本行给出。
                int addressY = 325; // 声明 addressY：其他地址列表的当前纵坐标；按右侧表达式初始化。
                for (std::size_t i = 1; session.showOtherAddresses && i < addresses.size(); ++i) // 循环推进：`std::size_t i = 1; session.showOtherAddresses && i < addresses.size(); ++i)`；逐项处理棋格、方向、字符或列表元素。
                { // 开始上方函数、条件、循环或类型的作用域。
                    const std::string& address = addresses[i]; // 声明 address：IPv4 地址结构或地址文字；按右侧表达式初始化。
                    const std::wstring next(address.begin(), address.end()); // 声明 next：下一个地址显示文字；参数或长度由本行给出。
                    if (!lineText.empty() && textwidth((lineText + L"   " + next).c_str()) > 640) // 检查 `if (!lineText.empty() && textwidth((lineText + L"   " + next).c_str()) > 640)`；条件成立时执行括号之后或下一行的处理。
                    { // 开始上方函数、条件、循环或类型的作用域。
                        outtextxy(80, addressY, lineText.c_str()); // 在指定坐标绘制文字；参数为 `80, addressY, lineText.c_str())`。
                        addressY += 25; // 更新数据：`addressY += 25`；赋值后的状态供后续逻辑或绘图使用。
                        lineText.clear(); // 清空 `lineText.clear()`，避免旧状态残留。
                        if (addressY > 350) break; // 检查 `if (addressY > 350) break`；条件成立时执行括号之后或下一行的处理。
                    }
                    if (!lineText.empty()) lineText += L"   "; // 检查 `if (!lineText.empty()) lineText += L"   "`；条件成立时执行括号之后或下一行的处理。
                    lineText += next; // 更新数据：`lineText += next`；赋值后的状态供后续逻辑或绘图使用。
                }
                if (!lineText.empty()) outtextxy(80, addressY, lineText.c_str()); // 检查 `if (!lineText.empty()) outtextxy(80, addressY, lineText.c_str())`；条件成立时执行括号之后或下一行的处理。
                if (session.showOtherAddresses) // 检查 `if (session.showOtherAddresses)`；条件成立时执行括号之后或下一行的处理。
                    outtextxy(80, 375, L"若上方 IP 无法连接，可选与加入方处于同一网络的其他地址"); // 在指定坐标绘制文字；参数为 `80, 375, L"若上方 IP 无法连接，可选与加入方处于同一网络的其他地址")`。
            }
        }
        if (session.network.state() == NetworkState::CONNECTED && !session.choicePending) // 检查 `if (session.network.state() == NetworkState::CONNECTED && !session.choicePending)`；条件成立时执行括号之后或下一行的处理。
        { // 开始上方函数、条件、循环或类型的作用域。
            drawButton(180, 410, 200, 45, L"选择黑棋", mouseX, mouseY); // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
            drawButton(420, 410, 200, 45, L"选择白棋", mouseX, mouseY); // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
        }
        setfont(16, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `16, 0, L"微软雅黑")`。
        if (session.network.state() == NetworkState::CONNECTED) // 检查 `if (session.network.state() == NetworkState::CONNECTED)`；条件成立时执行括号之后或下一行的处理。
            outtextxy(170, 390, L"一方选色后另一方自动分配；同时选择以房主为准"); // 在指定坐标绘制文字；参数为 `170, 390, L"一方选色后另一方自动分配；同时选择以房主为准")`。
        drawButton(280, 465, 240, 48, L"返回菜单", mouseX, mouseY); // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
        if (!session.network.error().empty()) // 检查 `if (!session.network.error().empty())`；条件成立时执行括号之后或下一行的处理。
        { // 开始上方函数、条件、循环或类型的作用域。
            const std::string& error = session.network.error(); // 声明 error：错误原因；按右侧表达式初始化。
            const std::wstring message(error.begin(), error.end()); // 声明 message：待显示的提示文字；参数或长度由本行给出。
            outtextxy(28, 550, message.c_str()); // 在指定坐标绘制文字；参数为 `28, 550, message.c_str())`。
        }
        return; // 提前结束当前无返回值函数，避免继续执行后续处理。
    }
    setfont(34, 0, L"微软雅黑"); setbkmode(TRANSPARENT); // 设置后续文字的字体和字号；参数为 `34, 0, L"微软雅黑"); setbkmode(TRANSPARENT)`。
    setcolor(EGERGB(45, 37, 26)); // 设置后续线条和文字颜色；参数为 `EGERGB(45, 37, 26))`。
    outtextxy(310, 95, L"联机对决"); // 在指定坐标绘制文字；参数为 `310, 95, L"联机对决")`。
    setfont(18, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `18, 0, L"微软雅黑")`。
    outtextxy(180, 155, L"创建房间无需填写 IP，创建后显示房主地址"); // 在指定坐标绘制文字；参数为 `180, 155, L"创建房间无需填写 IP，创建后显示房主地址")`。
    drawButton(280, 205, 240, 48, L"创建房间", mouseX, mouseY); // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
    setfont(16, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `16, 0, L"微软雅黑")`。
    outtextxy(240, 260, L"房主 IP（仅加入房间时填写）"); // 在指定坐标绘制文字；参数为 `240, 260, L"房主 IP（仅加入房间时填写）")`。
    const std::wstring ip(session.serverIp.begin(), session.serverIp.end()); // 声明 ip：IPv4 数值或显示文字；参数或长度由本行给出。
    drawButton(240, 285, 320, 48, // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
               ip.empty() && !session.editingIp ? L"请输入房主 IP" : ip.c_str(), mouseX, mouseY); // 条件表达式：`ip.empty() && !session.editingIp ? L"请输入房主 IP" : ip.c_str(), mouseX, mouseY)`；根据条件选择两个值之一。
    if (session.editingIp) // 检查 `if (session.editingIp)`；条件成立时执行括号之后或下一行的处理。
    { // 开始上方函数、条件、循环或类型的作用域。
        // 光标与输入框文字使用相同字体和居中起点。
        setfont(24, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `24, 0, L"微软雅黑")`。
        setcolor(EGERGB(150, 205, 255)); // 设置后续线条和文字颜色；参数为 `EGERGB(150, 205, 255))`。
        rectangle(243, 288, 557, 330); // 绘制矩形边框；参数为 `243, 288, 557, 330)`。
        if ((GetTickCount() / 500) % 2 == 0) // 每500毫秒切换光标显示状态，得到输入框闪烁效果。
        { // 开始上方函数、条件、循环或类型的作用域。
            const int caretX = 240 + (320 - textwidth(ip.c_str())) / 2 + // 声明 caretX：输入光标横坐标；按右侧表达式初始化。
                textwidth(ip.substr(0, session.ipCursor).c_str()); // 测量文字宽度，用于居中或换行；参数为 `ip.substr(0, session.ipCursor).c_str())`。
            line(caretX, 296, caretX, 322); // 绘制直线段；参数为 `caretX, 296, caretX, 322)`。
        }
    }
    setfont(18, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `18, 0, L"微软雅黑")`。
    setcolor(EGERGB(45, 37, 26)); // 设置后续线条和文字颜色；参数为 `EGERGB(45, 37, 26))`。
    outtextxy(180, 340, session.editingIp ? L"输入房主 IP，退格删除，Enter 加入" : L"填写房主创建房间后显示的 IP，然后加入"); // 在指定坐标绘制文字；参数为 `180, 340, session.editingIp ? L"输入房主 IP，退格删除，Enter 加入" : L"填写房主创建房间后显示的 IP，然后加入")`。
    drawButton(280, 385, 240, 48, L"加入房间", mouseX, mouseY); // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
    drawButton(280, 465, 240, 48, L"返回菜单", mouseX, mouseY); // 根据鼠标是否悬停设置按钮颜色，绘制圆角背景及居中文字。
    if (session.network.state() == NetworkState::FAILED || // 检查 `if (session.network.state() == NetworkState::FAILED ||`；条件成立时执行括号之后或下一行的处理。
        session.network.state() == NetworkState::DISCONNECTED) // 承接上方表达式的参数、条件或初值：`session.network.state() == NetworkState::DISCONNECTED)`。
    { // 开始上方函数、条件、循环或类型的作用域。
        setfont(16, 0, L"微软雅黑"); // 设置后续文字的字体和字号；参数为 `16, 0, L"微软雅黑")`。
        outtextxy(180, 525, status(session)); // 在指定坐标绘制文字；参数为 `180, 525, status(session))`。
        const std::string& error = session.network.error(); // 声明 error：错误原因；按右侧表达式初始化。
        const std::wstring message(error.begin(), error.end()); // 声明 message：待显示的提示文字；参数或长度由本行给出。
        outtextxy(28, 550, message.c_str()); // 在指定坐标绘制文字；参数为 `28, 550, message.c_str())`。
    }
}
bool onlineClick(OnlineSession& session, int x, int y) // 函数入口：处理联机界面的返回、创建、加入、选色、地址展开和棋盘点击。
{ // 开始上方函数、条件、循环或类型的作用域。
    if (session.localPiece != EMPTY) // 检查 `if (session.localPiece != EMPTY)`；条件成立时执行括号之后或下一行的处理。
    { // 开始上方函数、条件、循环或类型的作用域。
        if (matchBack(x, y)) { session.reset(); return true; } // 检查 `if (matchBack(x, y)) { session.reset(); return true; }`；条件成立时执行括号之后或下一行的处理。
        int row, col; // 声明 row：棋盘行号；参数或长度由本行给出。
        if (mouseToBoard(x, y, row, col)) session.move(row, col); // 检查 `if (mouseToBoard(x, y, row, col)) session.move(row, col)`；条件成立时执行括号之后或下一行的处理。
    }
    else if (session.inRoom) // 前一条件不满足时，再检查 `(session.inRoom)`。
    { // 开始上方函数、条件、循环或类型的作用域。
        if (insideButton(x, y, 280, 465, 240, 48)) { session.reset(); return true; } // 检查 `if (insideButton(x, y, 280, 465, 240, 48)) { session.reset(); return true; }`；条件成立时执行括号之后或下一行的处理。
        if (session.isHost && session.network.hostAddresses().size() > 1 && // 检查 `if (session.isHost && session.network.hostAddresses().size() > 1 &&`；条件成立时执行括号之后或下一行的处理。
            insideButton(x, y, 520, 250, 200, 40)) // 检查坐标是否落在按钮矩形内，采用左上含、右下不含的范围。
            session.showOtherAddresses = !session.showOtherAddresses; // 更新数据：`session.showOtherAddresses = !session.showOtherAddresses`；赋值后的状态供后续逻辑或绘图使用。
        if (insideButton(x, y, 180, 410, 200, 45)) session.chooseColor(BLACK_PIECE); // 检查 `if (insideButton(x, y, 180, 410, 200, 45)) session.chooseColor(BLACK_PIECE)`；条件成立时执行括号之后或下一行的处理。
        else if (insideButton(x, y, 420, 410, 200, 45)) session.chooseColor(WHITE_PIECE); // 前一条件不满足时，再检查 `(insideButton(x, y, 420, 410, 200, 45)) session.chooseColor(WHITE_PIECE)`。
    }
    else // 前面的条件不满足时，执行下面的备用分支。
    { // 开始上方函数、条件、循环或类型的作用域。
        if (insideButton(x, y, 280, 465, 240, 48)) { session.reset(); return true; } // 检查 `if (insideButton(x, y, 280, 465, 240, 48)) { session.reset(); return true; }`；条件成立时执行括号之后或下一行的处理。
        session.editingIp = insideButton(x, y, 240, 285, 320, 48); // 更新数据：`session.editingIp = insideButton(x, y, 240, 285, 320, 48)`；赋值后的状态供后续逻辑或绘图使用。
        if (session.editingIp) session.ipCursor = session.serverIp.size(); // 检查 `if (session.editingIp) session.ipCursor = session.serverIp.size()`；条件成立时执行括号之后或下一行的处理。
        if (insideButton(x, y, 280, 205, 240, 48)) session.create(); // 检查 `if (insideButton(x, y, 280, 205, 240, 48)) session.create()`；条件成立时执行括号之后或下一行的处理。
        else if (insideButton(x, y, 280, 385, 240, 48)) session.join(); // 前一条件不满足时，再检查 `(insideButton(x, y, 280, 385, 240, 48)) session.join()`。
    }
    return false; // 当前点击处理结束，继续留在此页面；不是操作失败的标记。
}
void onlineKey(OnlineSession& session, int key) // 函数入口：编辑 IPv4 输入框光标及数字点号，Enter 发起加入。
{ // 开始上方函数、条件、循环或类型的作用域。
    if (!session.editingIp || session.inRoom) return; // 检查 `if (!session.editingIp || session.inRoom) return`；条件成立时执行括号之后或下一行的处理。
    if (session.ipCursor > session.serverIp.size()) session.ipCursor = session.serverIp.size(); // 检查 `if (session.ipCursor > session.serverIp.size()) session.ipCursor = session.serverIp.size()`；条件成立时执行括号之后或下一行的处理。
    if (key == 8 && session.ipCursor > 0) session.serverIp.erase(--session.ipCursor, 1); // 检查 `if (key == 8 && session.ipCursor > 0) session.serverIp.erase(--session.ipCursor, 1)`；条件成立时执行括号之后或下一行的处理。
    else if (key == 256 + 83 && session.ipCursor < session.serverIp.size()) // 前一条件不满足时，再检查 `(key == 256 + 83 && session.ipCursor < session.serverIp.size())`。
        session.serverIp.erase(session.ipCursor, 1); // 移除已处理字节或输入字符：`session.serverIp.erase(session.ipCursor, 1)`。
    else if (key == 256 + 75 && session.ipCursor > 0) --session.ipCursor; // 前一条件不满足时，再检查 `(key == 256 + 75 && session.ipCursor > 0) --session.ipCursor`。
    else if (key == 256 + 77 && session.ipCursor < session.serverIp.size()) ++session.ipCursor; // 前一条件不满足时，再检查 `(key == 256 + 77 && session.ipCursor < session.serverIp.size()) ++session.ipCursor`。
    else if (key == 256 + 71) session.ipCursor = 0; // 前一条件不满足时，再检查 `(key == 256 + 71) session.ipCursor = 0`。
    else if (key == 256 + 79) session.ipCursor = session.serverIp.size(); // 前一条件不满足时，再检查 `(key == 256 + 79) session.ipCursor = session.serverIp.size()`。
    else if (((key >= '0' && key <= '9') || key == '.') && session.serverIp.size() < 15) // 前一条件不满足时，再检查 `(((key >= '0' && key <= '9') || key == '.') && session.serverIp.size() < 15)`。
        session.serverIp.insert(session.ipCursor++, 1, static_cast<char>(key)); // 递增或递减 `session.serverIp.insert(session.ipCursor++, 1, static_cast<char>(key))`，推进计数、坐标或光标。
    else if (key == 13) session.join(); // 前一条件不满足时，再检查 `(key == 13) session.join()`。
}
