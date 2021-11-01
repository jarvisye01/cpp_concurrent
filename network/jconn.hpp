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
    inline int Avai() const;
    inline int Rest() const;
    void Reset();

private:
    int TailSpace() const;
    void Move();
    void AdjustSpace(int n);
    int size;
    int rp, wp;
    char *buffer;
};

struct JTcpConn
{
    int sockfd;
    in_addr_t clientAddr;
    in_port_t clientPort;

    JTcpConn(int fd, in_addr_t addr, in_port_t port);

    // Send to network
    int Send(const char *buf, int sz, char *err);
    // Receive from network
    int Recv(char *buf, int sz, char *err);
    // add to buffer
    int Append(const char *buf, int sz, char *err);

    JNetBuffer *input;
    JNetBuffer *output;
};

};

#endif