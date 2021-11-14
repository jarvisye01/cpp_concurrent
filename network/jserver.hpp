#ifndef J_TCPSERVER_H
#define J_TCPSERVER_H

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "network/jeventloop.hpp"
#include "concurrent/jthread.hpp"

namespace jarvis
{

class JTcpServer
{
public:
    enum SvrState
    {
        CREATE,
        INIT,
        BIND,
        LISTEN,
        RUN,
        STOP,
    };
    JTcpServer(const std::string h = "127.0.0.1", const std::string s = "8080", int n = 4);
    ~JTcpServer();
    JTcpServer(const JTcpServer&) = delete;
    JTcpServer& operator=(const JTcpServer&) = delete;

    int Init();
    void SetEventCallBack(JEventLoop::EventCallBack c);
    void SetTimeCallBack(JEventLoop::TimeCallBack c, uint64_t interval = 3000); // 默认3秒处理一次时间事件
    int Run();

private:
    int Bind();
    int Listen();
    static int WorkFunc(JEventLoop *e);
    int StartEL();
    SvrState state;
    std::string host;
    std::string service;
    int sockfd;
    sockaddr_in addr;
    bool running;

    int en;
    JEventLoop **els;
    JThread **workers;
    JEventLoop::EventCallBack ec;
    uint64_t tcInterval;
    JEventLoop::TimeCallBack tc;
};

}  // namespace jarvis

#endif