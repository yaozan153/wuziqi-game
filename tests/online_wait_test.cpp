#include <ege.h>
#include "../modes/online/online.h"
#include <cassert>
#include <iostream>

int main()
{
    ege::initgraph(800, 600, ege::INIT_RENDERMANUAL);
    ege::PIMAGE frame = ege::newimage(800, 600);
    OnlineSession host;
    assert(host.create(18889));
    // 等待超过连接握手的十秒超时，房主仍应保持监听。
    for (int i = 0; i < 720; ++i)
    {
        host.update();
        assert(host.inRoom && host.network.state() == NetworkState::LISTENING);
        ege::settarget(frame);
        ege::cleardevice();
        drawOnlineScreen(host, -1000, -1000);
        ege::settarget(NULL);
        ege::putimage(0, 0, frame);
        ege::delay_fps(60);
    }
    assert(onlineClick(host, 300, 480));
    assert(!host.inRoom && host.network.state() == NetworkState::IDLE);
    assert(host.create(18889));
    OnlineSession guest;
    guest.serverIp = "127.0.0.1";
    assert(guest.join(18889));
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
    while ((host.network.state() != NetworkState::CONNECTED ||
            guest.network.state() != NetworkState::CONNECTED) &&
           std::chrono::steady_clock::now() < deadline)
    {
        host.update();
        guest.update();
        ege::delay_ms(1);
    }
    assert(host.network.state() == NetworkState::CONNECTED);
    assert(guest.network.state() == NetworkState::CONNECTED);
    ege::settarget(frame);
    drawOnlineScreen(host, -1000, -1000);
    ege::settarget(NULL);
    guest.reset();
    host.reset();
    ege::delimage(frame);
    ege::closegraph();
    std::cout << "Online waiting screen test passed\n";
}
