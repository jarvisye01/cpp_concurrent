#include <algorithm>
#include <string.h>
#include "network/jconn.hpp"

namespace jarvis
{

int Adjust(int n)
{
    int num = 1;
    while (num < n)
        num = num << 1;
    return num;
}

JNetBuffer::JNetBuffer(int n): size(Adjust(n)), rp(0), wp(0), buffer(new char[size])
{
}

JNetBuffer::JNetBuffer(const JNetBuffer & b): size(b.size), rp(b.rp), wp(b.wp)
{
    buffer = new char[size];
    strncpy(buffer + rp, b.buffer + b.rp, b.wp - b.rp);
}

JNetBuffer& JNetBuffer::operator=(const JNetBuffer & b)
{
    if (this == &b)
        return *this;
    if (buffer != NULL)
        delete [] buffer, buffer = NULL;
    size = b.size;
    rp = b.rp, wp = b.wp;
    buffer = new char[size];
    strncpy(buffer + rp, b.buffer + b.rp, b.wp - b.rp);
    return *this;
}

JNetBuffer::~JNetBuffer()
{
    if (buffer != NULL)
        delete [] buffer;
}

int JNetBuffer::Write(const char *buf, int sz, char *err)
{
    if (Rest() < sz)
    {
        AdjustSpace(Adjust(wp - rp + size) * 2);
    }
    else
    {
        if (TailSpace() < sz)
            Move();
    }
    strncpy(buffer + wp, buf, sz);
    wp += sz;
    return sz;
}

int JNetBuffer::Read(char *buf, int sz, char *err)
{
    int rn = std::min(sz, Avai());
    strncpy(buf, buffer + rp, rn);
    rp += sz;
    if (Avai() > 0 && Avai() < size / 4)
        AdjustSpace(size / 4);
    return rn;
}

inline int JNetBuffer::Avai() const
{
    return wp - rp;
}

inline int JNetBuffer::Rest() const
{
    return size - (wp - rp);
}

void JNetBuffer::Reset()
{
    rp = wp = 0;
}

int JNetBuffer::TailSpace() const
{
    return size - wp;
}

void JNetBuffer::Move()
{
    strncpy(buffer, buffer + rp, Avai());
    wp = wp - rp;
    rp = 0;
}

// resize buffer
void JNetBuffer::AdjustSpace(int n)
{
    // make a new buffer copy content to it
    char *newBuf = new char[n];
    strncpy(newBuf, buffer + rp, wp - rp);
    int len = wp - rp;
    size = n;
    delete [] buffer, buffer = NULL;
    rp = 0, wp = len;
    buffer = newBuf;
}

JTcpConn::JTcpConn(int fd, in_addr_t addr, in_port_t port): sockfd(fd), clientAddr(addr), clientPort(port)
{}

}; // namespace jarvis