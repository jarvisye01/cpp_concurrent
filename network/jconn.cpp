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
    memcpy(buffer + rp, b.buffer + b.rp, b.wp - b.rp);
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
    memcpy(buffer + rp, b.buffer + b.rp, b.wp - b.rp);
    return *this;
}

JNetBuffer::~JNetBuffer()
{
    if (buffer != NULL)
        delete [] buffer;
}

int JNetBuffer::Write(const char *buf, int sz, char *err)
{
    EntureSpace(sz);
    memcpy(buffer + wp, buf, sz);
    wp += sz;
    return sz;
}

int JNetBuffer::Read(char *buf, int sz, char *err)
{
    int rn = sz == -1 ? Avai() : std::min(sz, Avai());
    memcpy(buf, buffer + rp, rn);
    rp += rn;
    if (Avai() > 0 && Avai() < size / 4)
        AdjustSpace(std::min(size / 4, 16));
    return rn;
}

int JNetBuffer::RecvFd(int fd, int n, char *err)
{
    int total = 0;
    if (n == -1)
    {
        // 一直接收数据，直到没有或者错误
        int sz = 16;
        while (true)
        {
            // 调整空间
            EntureSpace(sz);
            int r = recv(fd, buffer + wp, sz, 0);
            if (r == 0)
            {
                // there is no data
                break;
            }
            else if (r < 0)
            {   
                // no block EAGAIN代表已经没有数据
                if (errno == EAGAIN)
                    break;
                else
                    return -1;  // some error
            }
            else
            {
                total += r;
                wp += r;
                sz *= 2;
            }
        }        
    }
    else
    {
        // 调整空间
        EntureSpace(n);
        while (total < n)
        {
            int r = recv(fd, buffer + wp, n - total, 0);
            if (r == 0)
            {
                break;
            }
            else if (r < 0)
            {
                break;
            }
            else
            {
                total += r;
                wp += r;
            }
        }
    }
    return total;
}

int JNetBuffer::SendFd(int fd, int n, char *err)
{
    int total = 0;
    n = std::min(n, Avai());
    while (total < n)
    {
        int w = send(fd, buffer + rp, n - total, 0);
        if (w < 0)
        {
            if (errno == EAGAIN)
                continue;
            else
                return -1;
        }
        else
        {
            total += w;
            rp += w;
        }
    }
    return total;
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
    memcpy(buffer, buffer + rp, Avai());
    wp = wp - rp;
    rp = 0;
}

int JNetBuffer::GetHeader(char *buf, int sz)
{
    if (Avai() < sz)
        return -1;
    memcpy(buf, buffer + rp, sz);
    return 0;
}

// resize buffer
void JNetBuffer::AdjustSpace(int n)
{
    // make a new buffer copy content to it
    char *newBuf = new char[n];
    memcpy(newBuf, buffer + rp, wp - rp);
    int len = wp - rp;
    size = n;
    delete [] buffer, buffer = NULL;
    rp = 0, wp = len;
    buffer = newBuf;
}

void JNetBuffer::EntureSpace(int n)
{
    if (TailSpace() >= n)
        return;
    if (Rest() >= n)
        Move();

    AdjustSpace(Adjust((Avai() + n) * 2));
}

JTcpConn::JTcpConn(int fd, in_addr_t addr, in_port_t port): sockfd(fd), clientAddr(addr), clientPort(port)
{
    input = new JNetBuffer;
    output = new JNetBuffer;
}

JTcpConn::~JTcpConn()
{
    if (input != NULL)
        delete input;
    if (output != NULL)
        delete output;
}

int JTcpConn::GetFd()
{
    return sockfd;
}

int JTcpConn::SendPkg(char *err)
{
    return output->SendFd(sockfd, output->Avai(), err);
}

int JTcpConn::RecvPkg(char *err)
{
    return input->RecvFd(sockfd, -1, err);
}

int JTcpConn::Send(const char *buf, int sz, char *err)
{
    return output->Write(buf, sz, err);
}

int JTcpConn::Recv(char *buf, int sz, char *err)
{
    return input->Read(buf, sz, err);
}

JTcpConn::CHECKRET JTcpConn::Check()
{
    char header[6];
    if (input->GetHeader(header, 6) == 0)
    {
        if (header[0] == 'y' && header[1] == 'e')
        {
            // check length
            uint32_t len = *(uint32_t*)(header + 2);
            len = ntohl(len);
            if (len <= input->Avai())   // 已经足够处理一次消息
                return SUCC;
            else
                return GOON;
        }
        else
        {
            return ERR;
        }
    }
    return GOON;
}

}; // namespace jarvis