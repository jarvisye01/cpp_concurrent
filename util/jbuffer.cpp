#include <algorithm>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <string.h>
#include <sys/socket.h>
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
    memcpy(buffer, b.buffer, size);
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
    memcpy(buffer, b.buffer, size);
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
        memcpy(buffer + wp, static_cast<const char*>(buf), wn);
    }
    else
    {
        int t = size - wp;
        if (t >= wn)
        {
            memcpy(buffer + wp, static_cast<const char*>(buf), wn);
        }
        else
        {
            memcpy(buffer + wp, static_cast<const char*>(buf), t);
            memcpy(buffer, static_cast<const char*>(buf) + t, wn - t);
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
        memcpy(static_cast<char*>(buf), buffer + rp, rn);
    }
    else
    {
        int t = size - rp;
        if (t > rn)
        {
            memcpy(static_cast<char*>(buf), buffer + rp, rn);
        }
        else
        {
            memcpy(static_cast<char*>(buf), buffer + rp, size - t);
            memcpy(static_cast<char*>(buf) + (size - t), buffer, rn - t);
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
    return avai;
}

std::string JBuffer::ToString() const
{
    const char *fmt = "{size: %d, rp: %d, wp: %d, avai: %d, rest: %d}";
    char buf[512];
    sprintf(buf, fmt, size, rp, wp, Avai(), Rest());
    return std::string(buf);
}

namespace jutil
{

int GetPow2Bound(int n)
{
    int num = 1;
    while (num < n)
        num = num << 1;
    return num;
}

// ===========exception=============
const char* JOutOfBoundExp::what() const noexcept
{
    return "out of bound error!";
}

JIntOptExp::JIntOptExp(int opp): op(opp) {}

const char* JIntOptExp::what() const noexcept
{
    return "int opt error!";
}

int JIntOptExp::GetIntOpt() const { return op; }

// ===========buffer===========
JBufferBase::JBufferBase(size_t initSize): rp(0), wp(0), size(initSize), data(new char[size])
{
    if (initSize <= 0)
        throw JIntOptExp(initSize);
}

JBufferBase::JBufferBase(const JBufferBase & jbuf): rp(jbuf.rp), wp(jbuf.wp), size(jbuf.size), data(new char[size])
{
    memcpy(data + rp, jbuf.data + rp, wp - rp);
}

JBufferBase& JBufferBase::operator=(const JBufferBase & jbuf)
{
    if (this == &jbuf)
        return *this;
    Reset();
    rp = jbuf.rp, wp = jbuf.wp, size = jbuf.size;
    data = new char [size];
    memcpy(data, jbuf.data + rp, wp - rp);
    return *this;
}

JBufferBase::~JBufferBase()
{
    if (data != NULL)
        delete [] data, data = NULL;
}

size_t JBufferBase::Write(const void * buf, size_t sz)
{
    EnsureSpace(sz);
    memcpy(data + wp, buf, sz);
    wp += sz;
    return sz;
}

size_t JBufferBase::Write(const std::string & str)
{
    return Write(str.c_str(), str.size());
}

size_t JBufferBase::Read(void * buf, size_t sz)
{
    int rn = std::min(sz, wp - rp);
    memcpy(buf, data + rp, rn);
    rp += rn;
    Expand(GetPow2Bound(wp - rp));
    return rn;
}

void JBufferBase::Reset()
{
    rp = wp = 0;
    size = 0;
    if (data != NULL)
        delete [] data, data = NULL;
}

size_t JBufferBase::Size() const
{
    return wp - rp;
}

size_t JBufferBase::Capacity() const
{
    return size;
}

void JBufferBase::EnsureSpace(size_t n)
{
    if (size - wp >= n) 
        return;
    if (size - (wp - rp) >= n)
    {
        Move();
        return;
    }
    Expand(GetPow2Bound(size + n) * 2);
}

void JBufferBase::Move()
{
    memcpy(data, data + rp, wp - rp);
    wp -= rp, rp = 0;
}

void JBufferBase::Expand(size_t n)
{
    if (n < 0)
        throw JIntOptExp(n);
    n = n < 4 ? 4 : n;
    char * tmp = new char[n];
    if (data != NULL)
    {
        memcpy(tmp, data + rp, wp - rp);
        delete [] data;
    }
    wp -= rp, rp = 0;
    data = tmp;
}

// ===========JGenIOBufferBase============
JGenIOBufferBase::JGenIOBufferBase(int h): handle(h)
{
}

JGenIOBufferBase::~JGenIOBufferBase()
{}

size_t JGenIOBufferBase::WriteTo(size_t sz)
{
    if (sz < 0)
        throw JIntOptExp(sz);
    size_t wn = std::min(sz, wp - rp);
    size_t widx = 0;
    while (widx < wn)
    {
        size_t ret = ::write(handle, data + rp, wn - widx);
        if (ret == 0) 
        {
            if (errno == EAGAIN || errno == EINTR)
                continue;
            break;
        }
        else if (ret < 0)
        {
            return -1;
        }
        rp += ret, widx += ret;
    }
    Expand(GetPow2Bound(wp - rp));
    return widx;
}

size_t JGenIOBufferBase::ReadFrom(size_t sz)
{
    if (sz < 0)
        throw JIntOptExp(sz);
    EnsureSpace(sz);
    size_t rn = sz, ridx = 0;
    while (ridx < rn)
    {
        int ret = ::read(handle, data + wp, rn - ridx);
        if (ret == 0)
        {
            if (errno == EAGAIN || errno == EINTR)
                continue;
            break;
        }
        else if (ret < 0)
        {
            return -1;
        }
        wp += ret, ridx += ret;
    }
    return ridx;
}

void JGenIOBufferBase::SetHandle(int h)
{
    if (h <= 0)
        throw JIntOptExp(h);
    handle = h;
}

// ===========JNetBuffer============
JNetBuffer::JNetBuffer(int h): JGenIOBufferBase(h)
{}

JNetBuffer::~JNetBuffer()
{}

size_t JNetBuffer::WriteTo(size_t sz)
{
    if (sz < 0)
        throw JIntOptExp(sz);
    size_t wn = std::min(sz, wp - rp);
    size_t widx = 0;
    while (widx < wn)
    {
        size_t ret = ::send(handle, data + rp + widx, wn - widx, 0);
        if (ret == 0)
        {
            if (errno == EAGAIN || errno == EINTR)
                continue;
            break;
        }
        else if (ret < 0)
        {
            return -1;
        }
        rp += ret, widx += ret;
    }
    Expand(GetPow2Bound(wp - rp));
    return widx;
}

size_t JNetBuffer::ReadFrom(size_t sz)
{
    if (sz < 0)
        throw JIntOptExp(sz);
    EnsureSpace(sz);
    size_t rn = sz, ridx = 0;
    while (ridx < rn)
    {
        size_t ret = ::recv(handle, data + wp, rn - ridx, 0);
        if (ret == 0)
        {
            if (errno == EAGAIN || errno == EINTR)
                continue;
            break;
        }
        else if (ret < 0)
        {
            return -1;
        }
        wp += ret, ridx += ret;
    }
    return ridx;
}

};  // namespace jutil

}   // namespace jarvis
