#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include "jmutex.hpp"
#include "util/jtime.hpp"
#include "file/jfile.hpp"

namespace jarvis
{

JMutex::JMutex()
{
    pthread_mutex_init(&mutex, NULL);
}

JMutex::~JMutex()
{
    pthread_mutex_destroy(&mutex);
}

int JMutex::Lock()
{
    if (pthread_mutex_lock(&mutex) == 0)
        return 0;
    return -1;
}

int JMutex::UnLock()
{
    if (pthread_mutex_unlock(&mutex) == 0)
        return 0;
    return -1;
}

int JMutex::TryLock(int mc)
{
    uint64_t timeNow = GetMillTime();
    while (GetMillTime() - timeNow < static_cast<uint64_t>(mc))
    {
        if (pthread_mutex_trylock(&mutex) == 0)
            return 0;
    }
    return -1;
}

pthread_mutex_t& JMutex::GetMutex()
{
    return mutex;
}

JMutexHelper::JMutexHelper(JMutex & m): mutex(m)
{
    assert(mutex.Lock() == 0);
}

JMutexHelper::~JMutexHelper()
{
    assert(mutex.UnLock() == 0);
}

JCond::JCond(JMutex & m): mutex(m)
{
    pthread_cond_init(&cond, NULL);
}

JCond::~JCond()
{
    pthread_cond_destroy(&cond);
}

int JCond::Lock()
{
    return mutex.Lock();
}

int JCond::Wait()
{
    if (pthread_cond_wait(&cond, &mutex.GetMutex()) == 0)
        return 0;
    return -1;
}

int JCond::Signal()
{
    if (pthread_cond_signal(&cond) == 0)
        return 0;
    return -1;
}

int JCond::Broadcast()
{
    if (pthread_cond_broadcast(&cond) == 0)
        return 0;
    return -1;
}

JCountLatch::JCountLatch(int c, JMutex & m):
count(c), mutex(m), cond(mutex)
{}

JCountLatch::~JCountLatch() {}

bool JCountLatch::HasDone()
{
    bool ret;
    {
        JMutexHelper h(mutex);
        ret = HasDoneNonLock();
    }
    return ret;
}

bool JCountLatch::HasDoneNonLock()
{
    return count == 0;
}

int JCountLatch::Done()
{
    {
        JMutexHelper h(mutex);
        if (--count == 0)
            cond.Signal();
    }
    return 0;
}

int JCountLatch::Wait()
{
    cond.Lock();
    while (!HasDoneNonLock())
        cond.Wait();
    return 0;
}

JBarrier::JBarrier(int c, JMutex & m):
count(c), mutex(m), cond(mutex)
{}

JBarrier::~JBarrier() {}

int JBarrier::Wait()
{
    JMutexHelper h(mutex);
    if (--count == 0)
    {
        cond.Broadcast();
    }
    else
    {
        while (count > 0)
            cond.Wait();
    }
    return 0;
}

JFileLock::JFileLock(int fd): lockFd(fd), status(1)
{
    if (lockFd < 0)
        status = 0;
}

JFileLock::~JFileLock()
{
}

int JFileLock::SetLock(flock * lock, int type, int where, int start, int len)
{
    if (lock == NULL)
        return -1;
    memset(lock, 0, sizeof(flock));
    lock->l_type = type;
    lock->l_whence = where;
    lock->l_start = start;
    lock->l_len = len;
    return 0;
}


int JFileLock::Lock()
{
    if (status == 0)
        return -1;
    struct flock tlock;
    SetLock(&tlock, F_WRLCK, SEEK_SET, 0, 1);
    return fcntl(lockFd, F_SETLKW, &tlock);
}

int JFileLock::UnLock()
{
    if (status == 0)
        return -1;
    struct flock tlock;
    SetLock(&tlock, F_UNLCK, SEEK_SET, 0, 1);
    return fcntl(lockFd, F_SETLKW, &tlock);
}

}   // namespace jarvis
