#include <algorithm>
#include <unistd.h>
#include "string.h"
#include "assert.h"
#include "util/jbuffer.hpp"

namespace jarvis
{

JBuffer::JBuffer(int sz): size(sz), buffer(new char[sz]), rp(0), wp(0), avai(0)
{
    assert(sz > 0);
}

JBuffer::JBuffer(const JBuffer & b): size(b.size), buffer(new char[b.size]), rp(b.rp), wp(b.wp), avai(0)
{
    // 优化手段，仅仅拷贝有用的部分
    strncpy(buffer, b.buffer, size);
}

JBuffer& JBuffer::operator=(const JBuffer & b)
{
    if (&b == this)
        return *this;
    if (buffer)
        delete buffer, buffer = NULL;
    size = b.size;
    rp = b.rp, wp = b.wp, avai = b.avai;
    buffer = new char[size];
    strncpy(buffer, b.buffer, size);
    return *this;
}

JBuffer::~JBuffer()
{
    if (buffer != NULL)
        delete buffer;
}

inline int JBuffer::Size() const
{
    return size;
}

inline int JBuffer::Avai() const
{
    return avai;
}

inline int JBuffer::Rest() const
{
    return size - Avai();
}

int JBuffer::Write(const void *buf, int n)
{
    int wn = std::min(n, Rest());
    if (wn == 0)
        return wn;  // 提前返回
    if (rp > wp)
    {
        strncpy(buffer + wp, static_cast<const char*>(buf), wn);
    }
    else
    {
        int t = size - wp;
        if (t >= wn)
        {
            strncpy(buffer + wp, static_cast<const char*>(buf), wn);
        }
        else
        {
            strncpy(buffer + wp, static_cast<const char*>(buf), t);
            strncpy(buffer, static_cast<const char*>(buf) + t, wn - t);
        }
    }
    avai += wn;
    wp = (wp + wn) % size;
    return wn;
}

int JBuffer::Read(void *buf, int n)
{
    int rn = std::min(n, Avai());
    if (rn == 0)
        return rn;  // 提前返回
    if (rp < wp)
    {
        strncpy(static_cast<char*>(buf), buffer + rp, rn);
    }
    else
    {
        int t = size - rp;
        if (t > rn)
        {
            strncpy(static_cast<char*>(buf), buffer + rp, rn);
        }
        else
        {
            strncpy(static_cast<char*>(buf), buffer + rp, size - t);
            strncpy(static_cast<char*>(buf) + (size - t), buffer, rn - t);
        }
    }
    avai -= rn;
    rp = (rp + rn) % size;
    return rn;
}

int JBuffer::Reset()
{
    avai = 0;
    rp = wp = 0;
    return 0;
}

int JBuffer::WriteTo(int fd)
{
    int avai = Avai();
    if (avai == 0)
        return 0;
    if (rp < wp)
    {
        write(fd, buffer + rp, avai);
    }
    else
    {
        write(fd, buffer + rp, size - rp);
        write(fd, buffer, wp);
    }

    Reset();
}

std::string JBuffer::ToString() const
{
    const char *fmt = "{size: %d, rp: %d, wp: %d, avai: %d, rest: %d}";
    char buf[512];
    sprintf(buf, fmt, size, rp, wp, Avai(), Rest());
    return std::string(buf);
}

}   // namespace jarvis