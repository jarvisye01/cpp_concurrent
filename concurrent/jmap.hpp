#ifndef J_MAP_H
#define J_MAP_H

#include <cstdio>
#include <map>
#include <utility>
#include "concurrent/jmutex.hpp"

namespace jarvis
{

template<class K, class V>
class JSyncMap
{
public:
    JSyncMap();
    V Get(K k);
    void Insert(K k, V v);
    void Delete(K v);
private:
    std::map<K, V> m;
    JMutex mutex;
};

template<class K, class V>
JSyncMap<K, V>::JSyncMap()
{}

template<class K, class V>
V JSyncMap<K, V>::Get(K k)
{ 
    jarvis::JMutexHelper helper(mutex);
    V v;
    auto iter = m.find(k);
    if (iter != m.end())
        v = iter->second;
    return v;
}

template<class K, class V>
void JSyncMap<K, V>::Insert(K k, V v)
{
    jarvis::JMutexHelper helper(mutex);
    auto iter = m.find(k);
    if (iter != m.end())
        m.erase(iter);
    m.insert(std::make_pair(k, v));
}

template<class K, class V>
void JSyncMap<K, V>::Delete(K k)
{
    jarvis::JMutexHelper helper(mutex);
    auto iter = m.find(k);
    if (iter != m.end())
        m.erase(iter);
}

}   // namespace jarvis

#endif
