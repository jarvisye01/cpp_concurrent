#include <inttypes.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <ostream>
#include <string.h>
#include "concurrent/jthread.hpp"
#include "concurrent/jmutex.hpp"
#include "file/jfile.hpp"
#include "util/jtime.hpp"

using namespace jarvis;

int TestMutex()
{
    int count = 0;
    JMutex mutex;
    JCountLatch latch(5, mutex);
    JMutex m2;
    std::shared_ptr<JThread> ptrs[5];
    uint64_t startTime = GetMillTime();
    for (int i = 0; i < 5; i++)
    {
        std::string name = std::string("thread");
        ptrs[i].reset(new JThread(std::function<void(void)>([&count, &latch, &m2, i] () {
            std::cout << "thread-" << i + 1 << " start." << std::endl;
            for (int j = 0; j < 100000000; j++)
            {
                JMutexHelper h(m2);
                count++;
            }
            latch.Done();
            std::cout << "thread-" << i + 1 << " done." << std::endl;
        }), name));
        ptrs[i]->Start();
        ptrs[i]->Detach();
    }

    std::cout << "main thread wait." << std::endl;
    latch.Wait();
    std::cout << "count: " << count << std::endl;
    std::cout << "time: " << GetMillTime() - startTime << std::endl;
    return 0;
}

// fcntl文件所依赖进程id，用多线程测试会有问题
// int TestFileLock()
// {
//     // create a file for lock
//     jfile::OpenFile("/root/test.lock");
//     int count = 0;
//     JMutex m;
//     JCountLatch latch(5,m);
//     std::shared_ptr<JThread> ptrs[5];
//     JFileLock flock("/root/test.lock");
//     uint64_t startTime = GetMillTime();
//     for (int i = 0; i < 5; i++)
//     {
//         std::string name("thread");
//         ptrs[i].reset(new JThread([&count, &latch,&flock, i] () {
//             std::cout << "thread-" << i + 1 << " start." << std::endl;
//             for (int j = 0; j < 100000000; j++)
//             {
//                 // JGeneralLockGuard<JFileLock> h(flock);
//                 count++;
//             }
//             std::cout << "thread-" << i + 1 << " done." << std::endl;
//             latch.Done();
//         }, name));
//         ptrs[i]->Start();
//         ptrs[i]->Detach();
//     }
// 
//     std::cout << "main thread wait." << std::endl;
//     latch.Wait();
//     std::cout << "count: " << count << std::endl;
//     std::cout << "time: " << GetMillTime() - startTime << std::endl;
//     return 0;
// }

int main(int argc, char ** argv)
{
    TestMutex();
    // TestFileLock();
}
