#include <cstddef>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include "network/jsocket.hpp"

int main(int argc, char ** argv)
{
    jarvis::jnet::JNetAddress addr(std::string("127.0.0.1"), 8000);
    jarvis::jnet::JServerSocket server(AF_INET, SOCK_STREAM, 0);

    server.Bind(addr);
    server.Listen();

    while (true)
    {
        // jarvis::jnet::JNetAddress clientAddr;
        int fd = server.Accept(NULL);
        if (fd < 0)
        {
            printf("accept error\n");
            continue;
        }
        char buf[1024];
        jarvis::jnet::JClientSocket client(fd);
        size_t sz = 0;

        printf("server start\n");
        sz = client.Recv(static_cast<void*>(buf), 12);
        buf[sz] = '\0';
        printf("server recv(%d): %s\n", strlen(buf), buf);
        sz = client.Send(static_cast<const void*>(buf), strlen(buf), true);
        printf("server send(%d): %s\n", strlen(buf), buf);
        client.ShutDown();
    }

    return 0;
}
