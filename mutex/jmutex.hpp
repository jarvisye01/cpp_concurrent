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
private:
    pthread_mutex_t mutex;
};

}   // namespace jarvis

#endif