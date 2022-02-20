#ifndef J_EVENTLOOP_H
#define J_EVENTLOOP_H

#include <pthread.h>
#include <sys/epoll.h>
#include <inttypes.h>
#include "network/jconn.hpp"
#include "network/jpoller.hpp"
#include "network/jsocket.hpp"

namespace jarvis
{

struct EpollState
{
    int fd;
    JTcpConn *conn;
    int event;
    uint64_t lastTriger; // 上一次触发的时间
    EpollState();
    void UpdateTriger();
    void Clear();
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
    void ClearNonActive();
    epoll_event* GetEvent(int idx);
    EpollState* GetEpollState(int idx);
private:
    int maxEvents;
    int epfd;
    // 原始的event结构还是太精简了，需要新的结构更多的字段
    epoll_event *events;
    EpollState *states;
};

class JEventLoop
{
public:
    typedef int (*EventCallBack)(JTcpConn *, JEventLoop *);
    typedef int (*TimeCallBack)(JEventLoop *);
    JEventLoop();
    JEventLoop(const JEventLoop&) = delete;
    JEventLoop& operator=(const JEventLoop&) = delete;
    JEpoller* GetEpoller();
    ~JEventLoop();

    int MainLoop();
    void SetEventCallBack(EventCallBack c);
    void SetTimeCallBack(TimeCallBack c, int interval);

    friend class JTcpServer;
private:
    // 上一次清理不活跃socket的时间
    uint64_t lastClear;
    int TimeFunc();
    JEpoller *epoller;
    bool isRunning;
    EventCallBack callback;
    uint64_t lastTC;    // last time event triger millsecond
    uint64_t tcInterval;
    TimeCallBack timeCallBack;
};

namespace jnet
{

/*
 * EventLoop实现了一个事件循环
 * 1.Loop
 * 2.SetPoller
 */
class JEventLoop
{
public:
    typedef int (*EventCallBack)(JClientSocket*, JEventLoop*);
    typedef int (*TimeCallBack)(JEventLoop*);

    JEventLoop();
    JEventLoop(const JEventLoop&) = delete;
    JEventLoop& operator=(const JEventLoop&) = delete;
    ~JEventLoop();

    int Loop();
    void SetPoller(jarvis::jnet::JPoller * p);
    void SetEventCallBack(EventCallBack callback);
    jarvis::jnet::JPoller* GetPoller();
private:
    jarvis::jnet::JPoller *poller;
    jarvis::jnet::JDataHandler *dataHandler;
    EventCallBack eventCallBack;
    TimeCallBack timeCallBack;
};

}   // namespace jnet

}  // namespace jarvis

#endif
