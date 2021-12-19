#include <stddef.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "concurrent/jshm.hpp"

namespace jarvis
{

// Exist check
bool ExistShm(key_t key)
{
    if (shmget(key, 0, IPC_EXCL | IPC_CREAT) == -1)
        return true;
    return false;
}

int GetShm(key_t key, size_t sz, int flag, bool * exist)
{
    bool e = ExistShm(key);
    int ret = 0;
    if (!e)
    {
        ret = shmget(key, sz, flag);
    }
    if (exist != NULL)
        *exist = e;
    return ret;
}

int SetShm(int shmId, struct shmid_ds *buf)
{
    return shmctl(shmId, IPC_SET, buf);
}

void* AttachShm(int shmId, int flag)
{
    return shmat(shmId, NULL, flag);
}

int DetachShm(void * addr)
{
    return shmdt(addr);
}

int DelShm(int shmId)
{
    return shmctl(shmId, IPC_RMID, NULL);
}

}   // namespace jarvis
