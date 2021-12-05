#include <functional>
#include <stdio.h>
#include <time.h>
#include "concurrent/jtask.hpp"
#include "util/jlog.hpp"

int print()
{
    INFO("time event print");
    return 0;
}

int hello()
{
    INFO("time print hello");
    return 0;
}

int TestTask()
{
    jarvis::jlog::OpenLog();
    jarvis::jlog::SetLogLevel(jarvis::jlog::TRACE);
    INFO("test time task");
    jarvis::JTimeTaskPool pool;
    // 测试的时候请将配置文件改为正确的路径 tNum = 4
    pool.Init("/root/task_test.conf");
    pool.Start();

    jarvis::JTimeTask tasks[10];
    for (int i = 0; i < 10; i++)
    {
        if (i % 2 == 0)
            tasks[i].SetFunc(std::function<int(void)>(print));
        else
            tasks[i].SetFunc(std::function<int(void)>(hello));

        tasks[i].SetRepeat(1000 * (i + 1));
        jarvis::print(tasks[i]);
        pool.RegistTask(tasks[i]);
    }

    while (true)
    {}
    return 0;
}

int main(int argc, char ** argv)
{
#ifdef TEST_TASK
    TestTask();
#endif
    return 0;
}
