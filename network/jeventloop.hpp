#ifndef J_EVENTLOOP_H
#define J_EVENTLOOP_H

#include <sys/epoll.h>
#include <inttypes.h>
#include "network/jconn.hpp"

namespace jarvis
{

struct EpollState
{
    int fd;
    JTcpConn *conn;
    int event;
    uint32_t lastTiger;
};

class JTcpServer;

class JEpoller
{
public:
    JEpoller(int me);
    JEpoller(const JEpoller&) = delete;
    JEpoller& operator=(const JEpoller&) = delete;
    int Create();
    int Register(int fd, JTcpConn *conn);
    int UnRegister(int fd);
    int AddEvent(int fd, int mask);
    int ModEvent(int fd, int mask);
    int DelEvent(int fd, int mask);
    int Wait(int t);
    epoll_event* GetEvent(int idx);
private:
    int maxEvents;
    int epfd;
    // 原始的event结构还是太精简了，需要新的结构更多的字段
    epoll_event *events;
    epoll_event *rgEvents;
};

class JEventLoop
{
public:
    JEventLoop();
    JEventLoop(const JEventLoop&) = delete;
    JEventLoop& operator=(const JEventLoop&) = delete;
    ~JEventLoop();

    int MainLoop();

    friend int EventCallBack(JTcpConn *, JEventLoop *);
    friend class JTcpServer;
private:
    JEpoller *epoller;
    bool isRunning;
};

int EventCallBack(JTcpConn *, JEventLoop *);

}  // namespace jarvis

#endif