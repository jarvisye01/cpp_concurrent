#ifndef J_QUEUE_H
#define J_QUEUE_H

#include <deque>
#include "concurrent/jmutex.hpp"

namespace jarvis
{

template<class T>
class JSyncQueue
{
public:
    JSyncQueue(int ms = 100);
    T Pop();
    // 0: success -1: fail
    int Push(T t);
    int Size();
private:
    int maxSize; 
    std::deque<T> q;
    JMutex mutex;
    JCond cond;
};

template<class T>
JSyncQueue<T>::JSyncQueue(int ms): maxSize(ms), mutex(), cond(mutex)
{}

template<class T>
T JSyncQueue<T>::Pop()
{
    JMutexHelper helper(mutex);
    while (q.size() <= 0)
    {
        cond.Wait();
    }
    auto t = q.front();
    q.pop_front();
    return t;
}

template<class T>
int JSyncQueue<T>::Push(T t)
{
    int ret = -1;
    JMutexHelper helper(mutex);
    if (q.size() < maxSize)
    {
        q.push_back(t);
        ret = 0;
    }
    cond.Broadcast();
    return ret;
}

template<class T>
int JSyncQueue<T>::Size()
{
    JMutexHelper helper(mutex);
    return q.size();
}

}   // jarvis

#endif