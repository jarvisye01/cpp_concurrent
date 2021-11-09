#include <unistd.h>
#include <string>
#include <assert.h>
#include <sys/epoll.h>
#include <string.h>
#include "network/jeventloop.hpp"
#include "util/jlog.hpp"

namespace jarvis
{

JEpoller::JEpoller(int me): maxEvents(me), events(new epoll_event[maxEvents]), rgEvents(new epoll_event[maxEvents])
{
    for (int i = 0; i < maxEvents; i++)
    {
        memset(&events[i], 0, sizeof(epoll_event));
        memset(&rgEvents[i], 0, sizeof(epoll_event));
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
    rgEvents[fd].data.ptr = conn;
    rgEvents[fd].events = 0;
    return 0;
}

int JEpoller::UnRegister(int fd)
{
    if (fd > maxEvents)
        return -1;
    rgEvents[fd].events = 0;
    JTcpConn *conn = (JTcpConn*)(rgEvents[fd].data.ptr);
    if (conn != NULL)
        delete conn;
    return 0;
}

int JEpoller::AddEvent(int fd, int mask)
{
    if (fd > maxEvents)
        return -1;

    int event = rgEvents[fd].events;
    int op = event == 0 ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    event |= mask;

    rgEvents[fd].events = event;

    epoll_event tmp{0};
    tmp.data.ptr = rgEvents[fd].data.ptr;
    tmp.events = event;
    return epoll_ctl(epfd, op, fd, &tmp);
}

int JEpoller::ModEvent(int fd, int mask)
{
    if (fd > maxEvents)
        return -1;
    
    int event = rgEvents[fd].events;
    event = mask;

    rgEvents[fd].events = event;

    epoll_event tmp{0};
    tmp.data.ptr = rgEvents[fd].data.ptr;
    tmp.events = event;
    return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &tmp);
}

int JEpoller::DelEvent(int fd, int mask)
{
    if (fd > maxEvents)
        return -1;
    
    int event = rgEvents[fd].events;
    if (mask == -1)
        event = 0;
    else
        event &= (~mask);

    rgEvents[fd].events = event;

    epoll_event tmp{0};
    tmp.data.ptr = rgEvents[fd].data.ptr;
    tmp.events = event;
    return epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &tmp);
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

JEventLoop::JEventLoop(): epoller(new JEpoller(1024))
{
    assert(epoller->Create() == 0);
}

JEventLoop::~JEventLoop()
{
    if (epoller != NULL)
        delete epoller;
}

int JEventLoop::MainLoop()
{
    isRunning = true;
    while (isRunning)
    {
       int num = epoller->Wait(-1);
       for (int i = 0; i < num; i++)
       {
            epoll_event *e = epoller->GetEvent(i);
            JTcpConn *conn = (JTcpConn*)(e->data.ptr);
            if (e->events & EPOLLIN)
            {       
                do
                {
                    int ret = conn->RecvPkg(NULL);
                    if (ret <= 0)
                    {
                        close(conn->GetFd());
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
                            int ret = EventCallBack(conn, this);
                            if (ret != 0)
                            {
                                close(conn->GetFd());
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
                        close(conn->GetFd());
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
                close(conn->GetFd());
                epoller->DelEvent(conn->GetFd(), -1);
                epoller->UnRegister(conn->GetFd());
            }
       } 
    }
    return 0;
}

int EventCallBack(JTcpConn *conn, JEventLoop *el)
{
    // read data
    char buf[1024];
    int ret = conn->Recv(buf, -1, NULL);
    buf[ret] = '\0';

    std::string msg = "hello, ";
    msg = msg + std::string(buf + 6);
    int len = msg.size() + 6;
    len = htonl(len);
    conn->Send("ye", 2, NULL);
    conn->Send((char *)&len, 4, NULL);
    conn->Send(msg.c_str(), msg.size(), NULL);
    el->epoller->AddEvent(conn->GetFd(), EPOLLOUT);
    return 0;
}

}  // namespace jarvis