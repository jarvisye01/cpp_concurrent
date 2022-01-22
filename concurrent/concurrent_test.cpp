#include <cstdio>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <ostream>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>
#include "concurrent/jthread.hpp"
#include "concurrent/jmutex.hpp"
#include "concurrent/jshm.hpp"
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
            printf("thread-%d start\n", i + 1);
            for (int j = 0; j < 100000000; j++)
            {
                JMutexHelper h(m2);
                count++;
            }
            latch.Done();
            printf("thread-%d done\n", i + 1);
        }), name));
        ptrs[i]->Start();
        ptrs[i]->Detach();
    }

    printf("main thread wait\n");
    latch.Wait();
    uint64_t diffTime = GetMillTime() - startTime;
    printf("count: %d\n", count);
    printf("time:  %lu\n", diffTime);
    return 0;
}

// 用多线程测试file lock似乎有点问题，file lock需要根据不同的进程id来判断锁的占有情况
int TestFileLock()
{
    int ret = 0;
    // create shares memory
    key_t skey = 1025;
    int shmId = 0;
    if ((shmId = shmget(skey, sizeof(int), IPC_CREAT)) <= 0)
    {
        std::cout << "get shared memory error shmdid " << shmId << std::endl;
        return -1;
    }

    // create a file for lock
    int lockFd = jfile::OpenFile("/root/temp.lock");
    int *count = NULL;
    count = (int*)shmat(shmId, count, 0);
    *count = 0;
    pid_t pids[5];
    uint64_t startTime = GetMillTime();
    for (int i = 0; i < 5; i++)
    {
        if ((pids[i] = fork()) < 0)
        {
            printf("fork error");
        }
        else if (pids[i] == 0)
        {
            printf("process %d start\n", getpid());
            JFileLock flock(lockFd);
            for (int j = 0; j < 100000000; j++)
            {
                JGeneralLockGuard<JFileLock> helper(flock);
                // if (j % 100000 == 0)
                //     std::cout << "process " << getpid() << " count " << *count << std::endl;    // 多进程使用file lock这样搞的性能堪忧
                (*count)++;
            }
            printf("process %d done\n", getpid());
            exit(0);
        }
        else
        {
            // parent process
            // std::cout << "parent fork a process" << std::endl;
        }
    }

    printf("main process wait for children\n");
    for (int i = 0; i < 5; i++)
    {
        printf("wait process %d\n", pids[i]);
        waitpid(pids[i], NULL, 0);
    }

    uint64_t diffTime = GetMillTime() - startTime;
    printf("count: %d\n", *count);
    printf("time: %lu\n", diffTime);

    if ((ret = shmdt(count)) != 0)
    {
        printf("delete shared memory error\n");
        return -1;
    }

    if ((ret = shmctl(shmId, IPC_RMID, 0)) != 0)
    {
        printf("remove shared memory error\n");
        return -1;
    }
    jfile::CloseFile(lockFd);
    return 0;
}

int TestShm()
{
    jarvis::JShmAllocator<int> allocator(1, 100);
    assert(allocator.Alloc() == 0);
    int & count = ((int*)allocator.GetAddr())[0];
    count = 0;

    // create a file for lock
    int lockFd = jfile::OpenFile("/root/temp.lock");
    pid_t pids[5];
    for (int i = 0; i < 5; i++)
    {
        if ((pids[i] = fork()) < 0)
        {
            printf("fork error");
        }
        else if (pids[i] == 0)
        {
            JFileLock flock(lockFd);
            for (int j = 0; j < 100000; j++)
            {
                JGeneralLockGuard<JFileLock> helper(flock);
                count++;
            }
            exit(0);
        }
        else
        {
            // parent process
        }
    }

    for (int i = 0; i < 5; i++)
    {
        printf("wait process %d\n", pids[i]);
        waitpid(pids[i], NULL, 0);
    }

    printf("count: %d\n", count);
    return 0;
}

int main(int argc, char ** argv)
{
    printf("start test concurrent component!\n");
    // cpu info: 4核8GB腾讯云服务器，这性能差的也太多了
    TestMutex();    // about 25s
    TestFileLock(); // about 1620s
    TestShm();
    return 0;
}
