#include <stddef.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "concurrent/jshm.hpp"

namespace jarvis
{

int GetShm(key_t key, size_t sz, int flag)
{
    int shmId = shmget(key, sz, flag);
    return shmId;
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
