#include "jmutex.hpp"
#include "util/jtime.hpp"

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
    while (GetMillTime() - timeNow < mc)
    {
        if (pthread_mutex_trylock(&mutex) == 0)
            return 0;
    }
    return -1;
}

}   // namespace jarvis