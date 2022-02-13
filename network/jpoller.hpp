#ifndef J_POLLER_H
#define J_POLLER_H

#include <sys/epoll.h>
#include <cstdint>
#include <vector>

namespace jarvis
{

/*
 * EventState用于表示fd的具体状态
 * 1.fd
 * 2.events
 * 3.lastTrigger
 */
struct JEventState
{
    int fd;
    uint32_t events;
    uint64_t lastTrigger;
    JEventState(int f = 0, uint32_t e = 0, uint64_t l = 0);
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
    virtual ~JPoller();

    // create poller
    int Create();
    // 多路复用io等待事件发生
    int Poll(int waitTime);
    int AddEvent(int fd, uint32_t events);
    int DelEvent(int fd, uint32_t events);
    epoll_event* GetEvent(int idx);
};

/*
 * Epoller使用epoll来实现多路复用
 */
class JEpoller: public JPoller
{
public:
    JEpoller(int max);
    ~JEpoller();

    int Create();
    int Poll(int waitTime);
    int AddEvent(int fd, uint32_t events);
    int DelEvent(int fd, uint32_t events);
    epoll_event* GetEvent(int idx);
private:
    int maxFd;
    int epollFd;
    std::vector<JEventState*> states;
    std::vector<epoll_event> epollEvents;
};

}   // namespace jarvis

#endif
