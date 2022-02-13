#ifndef J_POLLER_H
#define J_POLLER_H

#include <sys/epoll.h>
#include <cstdint>
#include <vector>

namespace jarvis
{

namespace jnet
{

/*
 * EventState用于表示fd的具体状态
 * 1.fd
 * 2.events
 * 3.lastTrigger
 * 4.ptr 可以附着一个指针给EventState
 */
struct JEventState
{
    static uint32_t NON_EVENTS;
    static uint32_t ALL_EVENTS;
    int fd;
    uint32_t events;
    uint64_t lastTrigger;
    void * ptr;
    JEventState(int f = 0, uint32_t e = 0, uint64_t l = 0, void * p = NULL);
};

/*
 * Poller是一个抽象接口，实现基本的多路IO服用操作
 * 1.Poll
 * 2.AddEvent
 * 3.DelEvent
 * 4.GetEvent
 */
class JPoller
{
public:
    virtual ~JPoller() {};

    // create poller
    virtual int Create() = 0;
    // 多路复用io等待事件发生
    virtual int Poll(int waitTime) = 0;
    virtual int AddEvent(int fd, uint32_t events) = 0;
    virtual int DelEvent(int fd, uint32_t events) = 0;
    virtual epoll_event* GetEvent(int idx) = 0;
};

/*
 * Epoller使用epoll来实现多路复用
 */
class JEpoller: public JPoller
{
public:
    JEpoller(int max);
    virtual ~JEpoller();

    virtual int Create();
    virtual int Poll(int waitTime);
    virtual int AddEvent(int fd, uint32_t events);
    virtual int DelEvent(int fd, uint32_t events);
    virtual epoll_event* GetEvent(int idx);
private:
    int maxFd;
    int epollFd;
    std::vector<JEventState*> states;
    std::vector<epoll_event> epollEvents;
};

}   // namespace jnet

}   // namespace jarvis

#endif
