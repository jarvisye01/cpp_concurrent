#include <unistd.h>
#include <functional>
#include <deque>
#include <vector>
#include "concurrent/jtask.hpp"
#include "concurrent/jmutex.hpp"
#include "concurrent/jthread.hpp"
#include "util/jlog.hpp"
#include "util/jtime.hpp"
#include "util/jconfig.hpp"


namespace jarvis
{

JTimeTask::JTimeTask(): tc(GetMillTime()), tt(SINGLE)
{}

JTimeTask::~JTimeTask()
{}

uint64_t JTimeTask::GetTaskID() const
{
    // 任务时间做id
    return tc;
}

uint64_t JTimeTask::GetTriggerTime() const
{
    return tc;
}

JTimeTask::TIMETASKTYPE JTimeTask::GetTimeTaskType()
{
    return tt;
}

uint64_t JTimeTask::GetNextTriggerTime()
{
    if (tt == SINGLE)
        return -1;
    tc += td;
    return tc;
}

bool JTimeTask::operator<(const JTimeTask & task) const
{
    if (this == &task)
        return false;
    return GetTriggerTime() > task.GetTriggerTime();
}

void JTimeTask::SetFunc(std::function<int(void)> func)
{
    f = func;
}

void JTimeTask::SetRepeat(uint64_t t)
{
    tt = REPEAT;
    td = t;
}

int JTimeTask::Run()
{
    return f();
}

JTimeTaskPool::JTimeTaskPool()
{
}

JTimeTaskPool::~JTimeTaskPool()
{
}

void JTimeTaskPool::Init(const std::string & config) 
{
    // read config
    JConfig::Init(config);
    tNum = JConfig::GetInt("thread_num");
    INFO("time process thread num %d", tNum);
    JConfig::Done();
}

int JTimeTaskPool::Start()
{
    threads = new JThread[tNum];
    processNum.resize(tNum);
    for (int i = 0; i < tNum; i++)
    {
        threads[i].SetName(std::string("time processor"));
        threads[i].SetThreadFunc(std::function<void(void)>([i, this] () {
            while (true)
            {
                JTimeTask task = execQ.Pop();
                INFO("trigger time %lu", task.GetTriggerTime());
                int t = task.GetNextTriggerTime();
                // repeat event
                if (t != -1)
                    RegistTask(task);
                int ret = task.Run();
                if (ret != 0)
                    ERROR("time event run error ret %d taskid %llu", ret, task.GetTaskID());
                processNum[i]++;
            }
        }));
        threads[i].Start();
        threads[i].Detach();
        INFO("time processor %s start", threads[i].GetName().c_str());
    }

    monitor = new JThread(std::function<void(void)>([this] () {
        std::vector<int> tmp(tNum, 0);
        while (true)
        {
            INFO("monitor process");
            {
                JMutexHelper helper(mutex);
                uint64_t now = GetMillTime();
                // INFO("now time %llu", now);
                while (waitQ.size() > 0 && waitQ.top().GetTriggerTime() <= now)
                {
                    JTimeTask task = waitQ.top();
                    waitQ.pop();
                    execQ.Push(task);
                }
            }
            // check thread active
            usleep(100 * 1000);
        }
    }), "monitor thread");

    monitor->Start();
    monitor->Detach();

    return 0;
}

int JTimeTaskPool::RegistTask(const JTimeTask & task)
{
    JMutexHelper helper(mutex);
    waitQ.push(task);
    return 0;
}

// 0: delete -1: not found
int JTimeTaskPool::UnRegistTask(uint64_t id)
{
    JMutexHelper helper(mutex);
    std::priority_queue<JTimeTask> tmp;
    bool exist = false;
    while (waitQ.size() > 0)
    {
        if (waitQ.top().GetTaskID() == id)
        {
            exist = true;
            continue;
        }
        tmp.push(waitQ.top());
        waitQ.pop();
    }

    waitQ = tmp;
    return exist ? 0 : -1;
}

}   // namespace jarvis
