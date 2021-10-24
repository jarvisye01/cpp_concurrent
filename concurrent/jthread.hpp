#ifndef J_THREAD_H
#define J_THREAD_H

#include <string>
#include <functional>
#include <pthread.h>

namespace jarvis
{

class JThread
{
public:
    enum THREADSTATE
    {
        INIT,
        RUNNING,
        STOP
    };
    typedef std::function<void (void)> ThreadFunc;
    JThread(ThreadFunc f, std::string n = std::string("default"));
    ~JThread();

    pthread_t GetTid() const;
    pid_t GetPid() const;
    const std::string& GetName() const;
    THREADSTATE GetState() const;

    int Start();
    int Join();
    int Detach();
private:
    typedef std::function<void*(void*)> PThreadFunc;
    std::string MakeName(const std::string & n);
    static void* ThreadRun(void* arg);

    pthread_t tid;
    pid_t pid;
    ThreadFunc func;
    std::string name;
    THREADSTATE state;
    bool isDetach;
};

}   // namespace jarvis

#endif