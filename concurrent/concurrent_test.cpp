#include <iostream>
#include <memory>
#include <string.h>
#include "concurrent/jthread.hpp"
#include "concurrent/jmutex.hpp"

using namespace jarvis;

int main(int argc, char ** argv)
{
    int count = 0;
    JMutex mutex;
    JCountLatch latch(5, mutex);
    JMutex m2;
    std::shared_ptr<JThread> ptrs[5];

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
    return 0;
}