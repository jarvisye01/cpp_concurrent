#ifndef J_SHM_H
#define J_SHM_H
#include <sys/ipc.h>
#include <sys/shm.h>

namespace jarvis
{

// Get shared memory
int GetShm(key_t key, size_t sz, int flag, bool * exist);

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
    if (shmId > 0)
    {
        DetachShm(data);
        DelShm(shmId);
    }
}

template<typename T>
int JShmAllocator<T>::Alloc()
{
    int ret = 0;
    if ((ret = GetShm(key, sz * sizeof(T), 0, NULL)) != 0)
        return ret;
    shmId = ret;
    if ((ret = AttachShm(shmId, 0)) != 0)
        return ret;
    return ret;
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
