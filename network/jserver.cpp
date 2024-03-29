#include <arpa/inet.h>
#include <functional>
#include <error.h>
#include <string.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <vector>
#include "concurrent/jthread.hpp"
#include "network/jeventloop.hpp"
#include "network/jpoller.hpp"
#include "network/jserver.hpp"
#include "network/jnetutil.hpp"
#include "network/jnetutil.hpp"
#include "network/jsocket.hpp"
#include "util/jrand.hpp"
#include "util/jlog.hpp"

namespace jarvis
{

JTcpServer::JTcpServer(const std::string h, const std::string s, int n):
state(CREATE), host(h), service(s), running(false), en(n), els(new JEventLoop*[en]), workers(new JThread*[en])
{
    TRACE("Server created.");
}

JTcpServer::~JTcpServer()
{
    if (els != NULL)
    {
        for (int i = 0; i < en; i++)
        {
            if (els[i] != NULL)
                delete els[i];
        }
        delete [] els;
    }
    if (workers != NULL)
    {
        for (int i = 0; i < en; i++)
        {
            if (workers[i] != NULL)
                delete workers[i];
        }
        delete [] workers;
    }
    TRACE("Server destroyed.");
}

int JTcpServer::Init()
{
    if (state != CREATE)
    {
        ERROR("Server is not at CREATE state.");
        return -1;
    }

    if (Resolve(host.c_str(), service.c_str(), &addr) != 0)
    {
        ERROR("Resolve host and service error host(%s) service(%s)", host.c_str(), service.c_str());
        return -1;
    }

    INFO("Server init port %d", ntohl(addr.sin_port));

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        ERROR("socket syscall error errno(%d) err(%s)", errno, strerror(errno));
        return -1;
    }

    if (SetSocketReuse(sockfd) != 0)
    {
        ERROR("SetSocketReuse error.");
        return -1;
    }

    state = INIT;

    if (Bind() != 0)
    {
        ERROR("Bind error");
        return -1;
    }

    if (Listen() != 0)
    {
        ERROR("Listen error");
        return -1;
    }
    
    return 0;
}

int JTcpServer::Bind()
{
    if (bind(sockfd, (sockaddr*)(&addr), sizeof(sockaddr_in)) < 0)
    {
        ERROR("bind syscall error errno(%d) err(%s)", errno, strerror(errno));
        return -1;
    }
    state = BIND;
    return 0;
}

int JTcpServer::Listen()
{
    if (state != BIND)
    {
        ERROR("Server is not at BIND state.");
        return -1;
    }
    if (listen(sockfd, 3) < -1)
    {
        ERROR("listen syscall error errno(%d) err(%s)", errno, strerror(errno));
        return -1;
    }
    state = LISTEN;
    return 0;
}

int JTcpServer::WorkFunc(JEventLoop *e)
{
    return e->MainLoop();
}

int JTcpServer::StartEL()
{
    for (int i = 0; i < en; i++)
    {
        if ((els[i] = new JEventLoop) != NULL)
        {
            els[i]->SetEventCallBack(ec);
            els[i]->SetTimeCallBack(tc, tcInterval);
        }
        else
        {
            ERROR("EventLoop[%d] id NULL", i);
        }

        if (els[i] != NULL)
        {
            workers[i] = new JThread(std::function<void(void)>(std::bind(WorkFunc, els[i])), 
                                        std::string("jserver worker"));
            workers[i]->Start();
            workers[i]->Detach();
            INFO("Server worker start.");
        }
        else
        {
            ERROR("Server els(%d) is NULL", i);
            return -1;
        }
    }
    return 0;
}

void JTcpServer::SetEventCallBack(JEventLoop::EventCallBack c)
{
    ec = c;
}

void JTcpServer::SetTimeCallBack(JEventLoop::TimeCallBack c, uint64_t interval)
{
    tc = c;
    tcInterval = interval;
}

int JTcpServer::Run()
{
    if (state != LISTEN)
    {
        ERROR("Server is not at LISTEN state.");
        return -1;
    }
    
    StartEL();

    TRACE("Server start to acccpt frome client.");
    running = true;
    state = RUN;
    while (running)
    {
        int clientFd;
        sockaddr_in clientAddr;
        socklen_t len = sizeof(sockaddr_in);
        if ((clientFd = accept(sockfd, (sockaddr*)&clientAddr, &len)) < 0)
            continue;

        SetNonBlock(clientFd);
        JTcpConn *conn = new JTcpConn(clientFd, clientAddr.sin_addr.s_addr, clientAddr.sin_port);
        // 随机添加到一个eventloop
        int idx = jrand::RandN(en);
        if (els[idx] != NULL)
        {
            TRACE("idx: %d\tclientFd %d", idx, clientFd);
            TRACE("Server accept clientIP(%s), clientPort(%d), clientFd(%d)", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), clientFd);
            els[idx]->epoller->Register(clientFd, conn);
            els[idx]->epoller->AddEvent(clientFd, EPOLLIN | EPOLLERR);
        }
    }
    state = STOP;
    return 0;
}

namespace jnet
{

// ==============JEasyTcpServer================
JEasyTcpServer::JEasyTcpServer(const std::string h, uint16_t p): host(h), port(p), workerSize(0), svrSocket(NULL)
{
}

JEasyTcpServer::~JEasyTcpServer()
{
    if (svrSocket != NULL)
        delete svrSocket;
}

int JEasyTcpServer::Init()
{
    int ret = -1;
    svrSocket = new jarvis::jnet::JServerSocket(AF_INET, SOCK_STREAM, 0);
    if (svrSocket == NULL)
        return -1;
    ret = svrSocket->Bind(jarvis::jnet::JNetAddress(host, port));
    if (ret != 0)
        return -1;
    ret = svrSocket->Listen();
    if (ret != 0)
        return -1;
    return 0;
}

int JEasyTcpServer::AddLooper(jarvis::jnet::JEventLoop * looper)
{
    workerSize++;
    loopers.push_back(looper);
    return workerSize;
}

int JEasyTcpServer::Run()
{
    // start eventloop
    std::vector<jarvis::JThread> workers;
    workers.resize(workerSize);
    for (int i = 0; i < workerSize; i++)
    {
        workers[i].SetThreadFunc([&, i] () {
            loopers[i]->Loop();
        });
        workers[i].Start();
        workers[i].Detach();
    }

    if (svrSocket == NULL)
        return -1;

    while (true)
    {
        int fd = svrSocket->Accept(NULL);
        jarvis::SetNonBlock(fd);
        jarvis::jnet::JClientSocket * socketPtr = new jarvis::jnet::JClientSocket(fd);
        int idx = jarvis::jrand::RandN(workerSize);
        if (idx >= 0 && idx < loopers.size() && loopers[idx] != NULL)
        {
            loopers[idx]->GetPoller()->AddEvent(fd, EPOLLIN | EPOLLERR | EPOLLHUP, static_cast<void*>(socketPtr)); 
        }
    }
    return 0;
}

}   // namespace jnet

}  // jarvis
