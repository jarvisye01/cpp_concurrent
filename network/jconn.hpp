#ifndef J_CONN_H
#define J_CONN_H

#include <netinet/in.h>

namespace jarvis
{

const int ERR_BUF = 256;

class JNetBuffer
{
public:
    JNetBuffer(int n = 16);
    JNetBuffer(const JNetBuffer & b);
    JNetBuffer& operator=(const JNetBuffer & b);
    ~JNetBuffer();
    int Write(const char *buf, int sz, char *err);
    int Read(char *buf, int sz, char *err);
    int RecvFd(int fd, int n, char *err);
    int SendFd(int fd, int n, char *err);
    inline int Avai() const;
    inline int Rest() const;
    int GetHeader(char *buf, int sz);
    void Reset();

private:
    int TailSpace() const;
    void Move();
    void AdjustSpace(int n);
    void EntureSpace(int n);
    int size;
    int rp, wp;
    char *buffer;
};

class JTcpConn
{
public:
    enum CHECKRET
    {
        SUCC,   // 成功
        GOON,   // 需要继续接收数据
        ERR,    // 出现错误
    };
    JTcpConn(int fd, in_addr_t addr, in_port_t port);
    JTcpConn(const JTcpConn&) = delete;
    JTcpConn& operator=(const JTcpConn&) = delete;
    ~JTcpConn();
    
    int GetFd();
    int Close();
    // Send to network
    int SendPkg(char *err);
    // Receive from network
    int RecvPkg(char *err);
    int Send(const char *buf, int sz, char *err);
    int Recv(char *buf, int sz, char *err);
    // 检查接受数据包的完整性
    CHECKRET Check();

private:
    int sockfd;
    in_addr_t clientAddr;
    in_port_t clientPort;

    JNetBuffer *input;
    JNetBuffer *output;
};

};

#endif