#include <unistd.h>
#include <string>
#include <assert.h>
#include <sys/epoll.h>
#include <string.h>
#include "network/jeventloop.hpp"
#include "util/jlog.hpp"
#include "util/jtime.hpp"

namespace jarvis
{

EpollState::EpollState(): fd(0), conn(NULL), event(0), lastTriger(GetMillTime())
{}

void EpollState::Clear()
{
    fd = 0;
    event = 0;
    lastTriger = GetMillTime();
    if (conn != NULL)
        delete conn;
    conn = NULL;
}

void EpollState::UpdateTriger()
{
    lastTriger = GetMillTime();
}

JEpoller::JEpoller(int me): maxEvents(me), events(new epoll_event[maxEvents]), states(new EpollState[maxEvents])
{
    for (int i = 0; i < maxEvents; i++)
    {
        memset(&events[i], 0, sizeof(epoll_event));
    }
}

int JEpoller::Create()
{
    if ((epfd = epoll_create(1024)) < 0)
        return -1;
    return 0;
}

int JEpoller::Register(int fd, JTcpConn *conn)
{
    if (fd > maxEvents)
        return -1;
    states[fd].fd = fd;
    states[fd].conn = conn;
    states[fd].UpdateTriger();
    return 0;
}

int JEpoller::UnRegister(int fd)
{
    if (fd > maxEvents)
        return -1;
    states[fd].Clear();
    return 0;
}

int JEpoller::AddEvent(int fd, int mask)
{
    if (fd > maxEvents)
        return -1;

    int op = states[fd].event == 0 ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    states[fd].event |= mask;

    epoll_event tmp{0};
    tmp.data.ptr = states[fd].conn;
    tmp.events = states[fd].event;
    return epoll_ctl(epfd, op, fd, &tmp);
}

int JEpoller::ModEvent(int fd, int mask)
{
    if (fd > maxEvents)
        return -1;
    
    states[fd].event = mask;

    epoll_event tmp{0};
    tmp.data.ptr = states[fd].conn;
    tmp.events = states[fd].event;
    return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &tmp);
}

int JEpoller::DelEvent(int fd, int mask)
{
    if (fd > maxEvents)
        return -1;
    
    epoll_event tmp{0};
    if (mask == -1)
    {
        // delete socket
        states[fd].Clear();
        return epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &tmp);
    }
    
    // delete event
    int event = states[fd].event & (~mask);
    return ModEvent(fd, event);
}

int JEpoller::Wait(int t)
{
    return epoll_wait(epfd, events, maxEvents, t);
}

epoll_event* JEpoller::GetEvent(int idx)
{
    if (maxEvents < idx)
        return NULL;
    return &events[idx];
}

EpollState* JEpoller::GetEpollState(int idx)
{
    if (maxEvents < idx)
        return NULL;
    return &states[idx];
}

// 清理不活跃的socket
void JEpoller::ClearNonActive()
{
    uint64_t now = GetMillTime();
    for (int i = 0; i < maxEvents; i++)
    {
        if (states[i].fd == 0)
            continue;
        if (now - states[i].lastTriger > 3 * 60 * 1000)
        {
            printf("now: %d, ladt %d\n", now, states[i].lastTriger);
            // delete fd from epfd
            close(states[i].fd);
            states[i].Clear();
            DelEvent(states[i].fd, -1);
        }
    }
}

JEventLoop::JEventLoop(): 
lastClear(GetMillTime()), isRunning(false), epoller(new JEpoller(1024)), callback(NULL), timeCallBack(NULL), lastTC(GetMillTime()), tcInterval(1000)
{
    assert(epoller->Create() == 0);
}

JEventLoop::~JEventLoop()
{
    if (epoller != NULL)
        delete epoller;
}

JEpoller* JEventLoop::GetEpoller()
{
    return epoller;
}

int JEventLoop::MainLoop()
{
    isRunning = true;
    while (isRunning)
    {
       int num = epoller->Wait(tcInterval);
       // process time events
       TimeFunc();
       for (int i = 0; i < num; i++)
       {
            epoll_event *e = epoller->GetEvent(i);
            JTcpConn *conn = (JTcpConn*)(e->data.ptr);
            if (conn == NULL)
                continue;
            epoller->GetEpollState(conn->GetFd())->UpdateTriger();
            if (e->events & EPOLLIN)
            {       
                do
                {
                    int ret = conn->RecvPkg(NULL);
                    if (ret <= 0)
                    {
                        conn->Close();
                        epoller->DelEvent(conn->GetFd(), -1);
                        epoller->UnRegister(conn->GetFd());
                        break;
                    }
                    JTcpConn::CHECKRET checkRet;
                    if (checkRet == JTcpConn::SUCC)
                    {
                        int i = 0;
                        while (i++ < 3 && checkRet == JTcpConn::SUCC)
                        {
                            int ret = callback(conn, this);
                            if (ret != 0)
                            {
                                conn->Close();
                                epoller->DelEvent(conn->GetFd(), -1);
                                epoller->UnRegister(conn->GetFd());
                                break;
                            }
                        }
                    }
                    else if (checkRet == JTcpConn::GOON)
                    {
                        // 继续接收数据
                        epoller->AddEvent(conn->GetFd(), EPOLLIN);
                    }
                    else    // ERR
                    {
                        conn->Close();
                        epoller->DelEvent(conn->GetFd(), -1);
                        epoller->UnRegister(conn->GetFd());
                    }
                } while (0);
                
            }

            if (e->events & EPOLLOUT)
            {
                conn->SendPkg(NULL);
                epoller->DelEvent(conn->GetFd(), EPOLLOUT);
            }

            if (e->events & EPOLLERR)
            {
                conn->Close();
                epoller->DelEvent(conn->GetFd(), -1);
                epoller->UnRegister(conn->GetFd());
            }
       } 
    }
    return 0;
}

void JEventLoop::SetEventCallBack(EventCallBack c)
{
    callback = c;
}

void JEventLoop::SetTimeCallBack(TimeCallBack c, int interval)
{
    timeCallBack = c;
    tcInterval = interval;
}

int JEventLoop::TimeFunc()
{
    TRACE("Process time events");
    uint64_t now = GetMillTime();
    if (now - lastClear > 120 * 1000)
    {
        epoller->ClearNonActive();
        TRACE("Clear non active socket");
        lastClear = GetMillTime();
    }
    
    if (now - lastTC > tcInterval && timeCallBack != NULL)
    {
        timeCallBack(this);
        lastTC = GetMillTime();
    }
    return 0;
}

}  // namespace jarvis