
#include <stdio.h>
#include <string>
#include <unistd.h>
#include "concurrent/jthread.hpp"
#include "util/jtime.hpp"

namespace jarvis
{

JThread::JThread(std::string n): name(MakeName(n))
{

}

JThread::JThread(ThreadFunc f, std::string n):
func(f), name(MakeName(n)), state(INIT), pid(getpid()), isDetach(false)
{

}

JThread::~JThread() {}


pthread_t JThread::GetTid() const
{
    return tid;
}

pid_t JThread::GetPid() const
{
    return pid;
}

const std::string& JThread::GetName() const
{
    return name;
}

JThread::THREADSTATE JThread::GetState() const
{
    return state;
}

void JThread::SetThreadFunc(ThreadFunc f)
{
    func = f;
}

void JThread::SetName(const std::string & n)
{
    name = n;
}

std::string JThread::MakeName(const std::string & n)
{
    char buf[100];
    uint64_t timeNow = GetMillTime();
    snprintf(buf, 100 - 1, "%lu-", timeNow);
    return std::string(buf) + n;
}

void* JThread::ThreadRun(void* arg)
{
    JThread *t = static_cast<JThread*>(arg);
    if (t != NULL)
    {
        t->func();
        t->state = STOP;
    }
    return NULL;
}

int JThread::Start()
{
    if (pthread_create(&tid, NULL, ThreadRun, this) == 0)
    {
        state = RUNNING;
        return 0;
    }
    return -1;
}

int JThread::Join()
{
    if (pthread_join(tid, NULL) == 0)
        return 0;
    return -1;
}

int JThread::Detach()
{
    if (pthread_detach(tid) == 0)
        return 0;
    return -1;
}

}  // namespace jarvis
