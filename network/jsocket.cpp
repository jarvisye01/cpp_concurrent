#include <algorithm>
#include <arpa/inet.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <netinet/in.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include "network/jsocket.hpp"
#include "network/jnetutil.hpp"
#include "util/jbuffer.hpp"

namespace jarvis
{

namespace jnet
{

// ==============JNetAddress================
int JNetAddress::ResolveAddrIpV4(const sockaddr_in * addr, std::string & ip)
{
    char buf[INET_ADDRSTRLEN];
    if ((inet_ntop(AF_INET, static_cast<const void*>(addr), buf, INET_ADDRSTRLEN)) == NULL)
        return -1;
    ip = buf;
    return 0;
}

int JNetAddress::ResolveStrIpV4(const std::string & ip, sockaddr_in * addr)
{
    return inet_pton(AF_INET, ip.c_str(), static_cast<void*>(addr));
}

JNetAddress::JNetAddress(const std::string & ip, uint16_t port)
{
    addrType = NET_IPv4;
    jarvis::Resolve(ip.c_str(), std::to_string(port).c_str(), &genAddr.ipv4Addr);
}

JNetAddress::JNetAddress()
{
}

JNetAddress::~JNetAddress()
{
}

int JNetAddress::GetDomain() const
{
    return domain;
}

void JNetAddress::SetDomain(int dm)
{
    domain = dm;
    if (domain == AF_INET)
    {
        // 暂时只考虑ipv4
        type = SOCK_STREAM;
        addrType = NET_IPv4;
    }
    else if (domain == AF_INET6)
    {
        addrType = NET_IPV6;
    }
    else if (domain == UNIX_LOCAL)
    {
    }
}

int JNetAddress::GetProtocolType() const
{
    return type;
}

sockaddr* JNetAddress::GetAddress()
{
    return const_cast<sockaddr*>(GetAddress());
}

const sockaddr* JNetAddress::GetAddress() const
{
    if (addrType == NET_IPv4)
        return (const sockaddr*)(&genAddr.ipv4Addr);
    else if (addrType == NET_IPV6)
        return (const sockaddr*)(&genAddr.ipv6Addr);
    else if (addrType == UNIX_LOCAL)
        return NULL;
    return NULL;
}

socklen_t JNetAddress::GetAddrLen() const
{
    if (addrType == NET_IPv4)
        return sizeof(sockaddr_in);
    else if (addrType == NET_IPV6)
        return sizeof(sockaddr_in6);
    else if (addrType == UNIX_LOCAL)
        return -1;
    return -1;
}

// ================JSocket===================
JSocket::JSocket(int domain, int type, int protocol): sendBuf(NULL), recvBuf(NULL)
{
    sockFd = socket(domain, type, protocol);
}

JSocket::JSocket(int fd): sockFd(fd), sendBuf(NULL), recvBuf(NULL)
{
}

JSocket::~JSocket()
{
    if (sendBuf != NULL)
        delete sendBuf;
    if (recvBuf != NULL)
        delete recvBuf;
}

int JSocket::GetSockFd() const
{
    return sockFd;
}

int JSocket::ShutDown(int how)
{
    if (sockFd < 0)
        return 0;
    return shutdown(sockFd, how);
}

int JSocket::Close()
{
    if (sockFd < 0)
    return close(sockFd);
}

size_t JSocket::GetSendBufSize() const
{
    return sendBuf != NULL ? sendBuf->Size() : 0;
}

size_t JSocket::GetRecvBufSize() const
{
    return recvBuf != NULL ? recvBuf->Size() : 0;
}

size_t JSocket::Write(const void * buf, size_t sz)
{
    if (sendBuf == NULL)
    {
        if ((sendBuf = new jarvis::jutil::JNetBuffer(sockFd)) == NULL)
            return -1;
    }
    return sendBuf->Write(buf, sz);
}

size_t JSocket::Read(void * buf, size_t sz)
{
    if (recvBuf == NULL)
    {
        if ((recvBuf = new jarvis::jutil::JNetBuffer(sockFd)) == NULL)
            return -1;
    }
    return recvBuf->Read(buf, sz);
}

size_t JSocket::Pick(void * buf, size_t sz)
{
    return recvBuf->Pick(buf, sz);
}

size_t JSocket::Send(size_t sz, bool again, bool * isAgain)
{
    if (sockFd <= 0)
        return -1;
    if (sendBuf == NULL)
    {
        if ((sendBuf = new jarvis::jutil::JNetBuffer(sockFd)) == NULL)
            return -1;
    }

    return sendBuf->WriteTo(sz, again, isAgain);
}

size_t JSocket::Recv(size_t sz, bool again, bool * isAgain)
{
    if (sockFd <= 0)
        return -1;
    if (recvBuf == NULL)
    {
        if ((recvBuf = new jarvis::jutil::JNetBuffer(sockFd)) == NULL)
            return -1;
    }

    return recvBuf->ReadFrom(sz, again, isAgain);
}

// =============JClientSocket===============
JClientSocket::JClientSocket(int domain, int type, int protocol): JSocket(domain, type, protocol)
{
}

JClientSocket::JClientSocket(int fd): JSocket(fd)
{
}

JClientSocket::~JClientSocket()
{
}

int JClientSocket::Connect(const JNetAddress & address)
{
    return connect(sockFd, address.GetAddress(), address.GetAddrLen());
}

// =============JServerSocket===============
JServerSocket::JServerSocket(int domain, int type, int protocol): JSocket(domain, type, protocol)
{
    // set server socket reuse
    jarvis::SetSocketReuse(sockFd);
}

JServerSocket::~JServerSocket()
{
}

int JServerSocket::Bind(const JNetAddress & address)
{
    return bind(sockFd, address.GetAddress(), address.GetAddrLen());
}

int JServerSocket::Listen()
{
    return listen(sockFd, 1024);
}

int JServerSocket::Accept(JNetAddress * clientAddr)
{
    int fd = 0;
    if (clientAddr == NULL)
    {
        fd = accept(sockFd, NULL, NULL);
    }
    else
    {
        clientAddr->SetDomain(AF_INET);
        socklen_t len = clientAddr->GetAddrLen();
        fd = accept(sockFd, clientAddr->GetAddress(), &len);
    }

    return fd;
}

// =============JEasyDataHandler===============
int JEasyDataHandler::HandleIn(jarvis::jnet::JClientSocket * socket)
{
    int recvSize = 1024, total = 0;
    while (true)
    {
        bool isAgain = false;
        int rz = socket->Recv(recvSize, false, &isAgain);

        if (rz == 0)
        {
            // client close socket
            break;
        }
        else if (rz < 0)
        {
            // error
            return -1;
        }
        else
        {
            // receive some data
            if (isAgain)
            {
                // non-blocked io
                break;
            }
            else
            {
                recvSize *= 2;
                total += rz;
                if (total >= 1024 * 1024)
                {
                    // data length is too long
                    return -1;
                }
            }
        }
    }
    return 0;
}

int JEasyDataHandler::HandleOut(jarvis::jnet::JClientSocket * socket)
{
    int ret = socket->Send(socket->GetSendBufSize(), true);
    return ret < 0 ? -1 : 0;
}

int JEasyDataHandler::CheckData(jarvis::jnet::JClientSocket * socket)
{
    if (socket->GetRecvBufSize() < 7)
    {
        // need to receive data
        return 1;
    }
    else
    {
        // pick front 7 bytes
        char buf[7];
        int pz = socket->Pick(buf, 7);
        if (pz < 7)
            return -1;

        // check magic num
        if (!(buf[0] == 'y' && buf[1] == 'x' && buf[2] == 'j'))
            return -1;

        // get length
        uint32_t netLen = 0, hostLen = 0;
        memcpy(static_cast<void*>(&netLen), static_cast<void*>(buf + 3), 4);
        hostLen = ntohl(netLen);

        if (hostLen + 7 == socket->GetRecvBufSize())
            return 0;
        else
            return -1;
    }
    return 0;
}

}   // namespace jet

}   // namespace jarvis
