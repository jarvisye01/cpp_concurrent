#ifndef J_TASK_H
#define J_TASK_H

#include <cstdint>
#include <cstdio>
#include <inttypes.h>
#include <functional>
#include <string>
#include <queue>
#include <utility>
#include <vector>
#include "concurrent/jmutex.hpp"
#include "concurrent/jqueue.hpp"
#include "concurrent/jthread.hpp"

namespace jarvis
{

class JTask
{
public:
    // every task has a id
    virtual uint64_t GetTaskID() const = 0;
    virtual int Run() = 0;
    virtual ~JTask() {}
};

// 基于时间的任务，taskid是时间戳，按照毫秒级的时间进行优先处理
// 最好可以支持重复事件
class JTimeTask: public JTask
{
public:
    enum TIMETASKTYPE
    {
        SINGLE,
        REPEAT,
    };
    JTimeTask();
    JTimeTask(uint64_t t);
    JTimeTask(uint64_t t1, TIMETASKTYPE t2, uint64_t t3, std::function<int(void)> func);
    // JTimeTask(const JTimeTask & task);
    // JTimeTask operator=(const JTimeTask & task);
    ~JTimeTask();
    // interface
    int Run();

    uint64_t GetTaskID() const;
    uint64_t GetTriggerTime() const;
    TIMETASKTYPE GetTimeTaskType();
    uint64_t GetNextTriggerTime();
    void SetFunc(std::function<int(void)> func);
    void SetRepeat(uint64_t t);
    bool operator<(const JTimeTask & task) const;

    friend void print(const JTimeTask & task);
private:
    uint64_t tc;
    TIMETASKTYPE tt;
    uint64_t td;    // 重复事件的时间间隔
    std::function<int(void)> f;
};

// bool operator<(const JTask & t1, const JTask & t2);

class JPool
{
public:
    virtual void Init(const std::string & config) = 0;
    virtual int Start() = 0;
    virtual ~JPool() {}
};

class JTimeTaskPool: public JPool
{
public:
    JTimeTaskPool();
    JTimeTaskPool(const JTimeTaskPool & p) = delete;
    JTimeTaskPool& operator=(const JTimeTaskPool & p) = delete;
    ~JTimeTaskPool();

    // interface
    void Init(const std::string & config);
    int Start();
    int RegistTask(const JTimeTask & task);
    int UnRegistTask(uint64_t id);
private:
    // 注册的事件
    std::priority_queue<JTimeTask> waitQ;
    JMutex mutex;   // guard for waitQ
    // 将要被执行的事件
    JSyncQueue<JTimeTask> execQ;

    // thread conf
    int tNum;
    JThread *threads;
    // 每个处理线程处理的任务个数
    std::vector<int> processNum;
    JThread *monitor;
};

// try template class
template<typename T>
class JGeneralTaskPool: public JPool
{
public:
    typedef T value_type;

    JGeneralTaskPool();
    ~JGeneralTaskPool();
    void Init(const std::string & config);
    int Start();
    int RegistTask(const value_type & task);
    int RegistTask(value_type && task);
    int UnRegistTask(uint64_t id);
private:
    std::priority_queue<value_type> waitQ;
    JMutex mutex;
    JSyncQueue<value_type> execQ;
    int tNum;
    JThread *threads;
    std::vector<int> processNum;
    JThread *monitor;
};

// some util function
void print(const JTimeTask & task);

}   // namespace jarvis
#endif
