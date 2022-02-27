#include "concurrent/jatomic.hpp"

namespace jarvis
{

// ==============JActomicInt================
JAtomicInt::JAtomicInt(int n): num(n)
{
}

JAtomicInt::~JAtomicInt()
{
}

int JAtomicInt::Add(int n)
{
    return __sync_add_and_fetch(&num, n);
}

int JAtomicInt::Sub(int n)
{
    return __sync_sub_and_fetch(&num, n);
}

int JAtomicInt::Get() const
{
    return num;
}

int JAtomicInt::Set(int n)
{
    int tmp;
    do
    {
        tmp = num;
    }
    while (!__sync_bool_compare_and_swap(&num, tmp, n));
    return n;
}

}   // namesapce jarvis
