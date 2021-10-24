#ifndef J_MUTEX_H
#define J_MUTEX_H
#include <pthread.h>

namespace jarvis
{

class JMutex
{
public:
    JMutex();
    ~JMutex();

    // 0: success -1: fail
    int Lock();
    int UnLock();
    // try lock(mc millseconds)
    int TryLock(int mc);

    pthread_mutex_t& GetMutex();
private:
    pthread_mutex_t mutex;
};

class JMutexHelper
{
public:
    JMutexHelper(JMutex & m);
    ~JMutexHelper();
private:
    JMutex & mutex;
};

class JCond
{
public:
    JCond(JMutex & m);
    ~JCond();

    // lock before wait
    int Lock();
    // 0: success -1: fail
    int Wait();
    // signal
    int Signal();
    // broadcast
    int Broadcast();
private:
    JMutex & mutex;
    pthread_cond_t cond;
};

class JCountLatch
{
public:
    JCountLatch(int c, JMutex & m);
    ~JCountLatch();

    int Done();
    int Wait();
    bool HasDone();
private:
    // before wait you should check hasdone
    bool HasDoneNonLock();
    volatile int count;
    JMutex & mutex;
    JCond cond;

};

class JBarrier
{
public:
    JBarrier(int c, JMutex & m);
    ~JBarrier();
    // wait for other threads
    int Wait();
private:
    volatile int count;
    JMutex & mutex;
    JCond cond;
};

}   // namespace jarvis

#endif