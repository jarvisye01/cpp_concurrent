#include <cstdint>
#include <sys/epoll.h>
#include "network/jpoller.hpp"
#include "util/jtime.hpp"

namespace jarvis
{

namespace jnet
{

// ============JEventState=============
JEventState::JEventState(int f, uint32_t e, uint64_t l, void * p): fd(f), events(e), lastTrigger(l), ptr(p)
{}

uint32_t JEventState::NON_EVENTS = 0;
uint32_t JEventState::ALL_EVENTS = 0xffffffff;

// =============JEpoller==============
JEpoller::JEpoller(int max): maxFd(max), epollFd(-1)
{
    states.reserve(maxFd);
    epollEvents.reserve(maxFd);
}

JEpoller::~JEpoller()
{
    for (int i = 0; i < states.size(); i++)
    {
        if (states[i] != NULL)
            delete states[i];
    }
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

}   // namespace jnet

}   // namespace jarvis
