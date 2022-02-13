#include <cstdint>
#include <sys/epoll.h>
#include "network/jpoller.hpp"
#include "util/jtime.hpp"

namespace jarvis
{

// ============JEventState=============
JEventState::JEventState(int f, uint32_t e, uint64_t l): fd(f), events(e), lastTrigger(l)
{}

// =============JEpoller==============
JEpoller::JEpoller(int max): maxFd(max), epollFd(-1)
{
    states.reserve(maxFd);
    epollEvents.reserve(maxFd);
}

int JEpoller::Create()
{
    epollFd = epoll_create(1024);
    return epollFd;
}

int JEpoller::Poll(int waitTime)
{
    if (epollFd == -1)
        return -1;
    return epoll_wait(epollFd, &epollEvents[0], epollEvents.size(), waitTime);
}

int JEpoller::AddEvent(int fd, uint32_t events) 
{
    if (fd >= maxFd)
        return -1;

    epoll_event e;
    if (states[fd] == NULL)
    {
        // add new event
        states[fd] = new JEventState(fd, events, jarvis::GetMillTime());
        // e.data.ptr = 
        e.events = events;
        return epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &e);
    }
    else
    {
        // mod an event
        states[fd]->events |= events;
        e.events = states[fd]->events;
        return epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &e);
    }
    return -1;
}

int JEpoller::DelEvent(int fd, uint32_t events)
{
    if (fd >= maxFd)
        return -1;

    epoll_event e;
    if (states[fd] == NULL)
    {
        return -1;
    }
    else
    {
        e.events = states[fd]->events & (~events);
        states[fd]->events = e.events;
        if (e.events)
        {
            // mod an event
            return epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &e);
        }
        else
        {
            // del an event
            delete states[fd];
            return epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, &e);
        }
    }
    return -1;
}

epoll_event* JEpoller::GetEvent(int idx)
{
    if (idx >= maxFd)
        return NULL;
    return &epollEvents[idx];
}

}   // namespace jarvis
