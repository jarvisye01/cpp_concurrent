#include <cstddef>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include "network/jnetutil.hpp"
#include "network/jsocket.hpp"

int main(int argc, char ** argv)
{
    jarvis::jnet::JNetAddress addr(std::string("127.0.0.1"), 8000);
    jarvis::jnet::JServerSocket server(AF_INET, SOCK_STREAM, 0);

    server.Bind(addr);
    server.Listen();

    while (true)
    {
        int fd = server.Accept(NULL);
        if (fd < 0)
        {
            printf("accept error\n");
            continue;
        }

        jarvis::SetNonBlock(fd);
        jarvis::jnet::JClientSocket client(fd);

        printf("server start\n");

        char buf[1024];
        size_t sz = 0;
        // non-block io must receive something
        while ((sz = client.Recv(1024, false)) == 0)
        {}
        client.Read(buf, sz);
        buf[sz] = '\0';
        printf("server recv(%d): %s\n", sz, buf);

        client.Write(buf, sz);
        sz = client.Send(sz, true);
        printf("server send(%d): %s\n", sz, buf);
        client.ShutDown();
    }

    return 0;
}
