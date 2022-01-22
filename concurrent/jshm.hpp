#ifndef J_SHM_H
#define J_SHM_H
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

namespace jarvis
{

// Get shared memory
int GetShm(key_t key, size_t sz, int flag);

// Set shared memory
int SetShm(int shmId, struct shmid_ds *buf);

// Attach shared memory
void* AttachShm(int shmId, int flag);

// Detach shared memory
int DetachShm(void * addr);

// Del share memory
int DelShm(int shmId);

template<typename T>
class JShmAllocator
{
public:
    JShmAllocator(int s, key_t k);
    ~JShmAllocator();
    int Alloc();
    void SetSize(int s);
    int Size() const;
    T* GetAddr();
private:
    int sz;
    key_t key;
    int shmId;
    T * data;
};

template<typename T>
JShmAllocator<T>::JShmAllocator(int s, key_t k): sz(s), key(k), shmId(-1)
{}

template<typename T>
JShmAllocator<T>::~JShmAllocator()
{
    if (shmId > 0 && data != NULL)
    {
        DetachShm(data);
        DelShm(shmId);
        shmId = -1;
    }
}

template<typename T>
int JShmAllocator<T>::Alloc()
{
    if ((shmId = GetShm(key, sz * sizeof(T), IPC_CREAT)) < 0)
        return -1;
    if ((data = static_cast<T*>(AttachShm(shmId, 0))) == NULL)
        return -1;
    return 0;
}

template<typename T>
void JShmAllocator<T>::SetSize(int s)
{
    if (s > 0) 
        sz = s;
}

template<typename T>
int JShmAllocator<T>::Size() const
{
    return sz;
}

template<typename T>
T* JShmAllocator<T>::GetAddr()
{
    return data;
}

}   // namespace jarvis

#endif
