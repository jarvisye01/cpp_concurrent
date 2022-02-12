#include <cstddef>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "network/jsocket.hpp"
#include "network/jnetutil.hpp"

int main(int argc, char ** argv)
{
    jarvis::jnet::JNetAddress addr(std::string("127.0.0.1"), 8000);
    jarvis::jnet::JClientSocket client(AF_INET, SOCK_STREAM, 0);
    client.Connect(addr);

    size_t sz = 0;
    char * buf = "hello, world";
    printf("client start\n");
    sz = client.Send(static_cast<const char*>(buf), strlen(buf), true);
    printf("client send(%d): %s\n", sz, buf);

    char recvBuf[1024];
    sz = 0;
    sz = client.Recv(static_cast<void*>(recvBuf), 12);
    recvBuf[sz] = '\0';
    printf("client recv(%d): %s\n", sz, recvBuf);
    return 0;
}
