#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

#include "network/jnetutil.hpp"

int main(int argc, char ** argv)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    jarvis::Resolve("127.0.0.1", "9000", &addr);
    int ret = connect(fd, (sockaddr*)&addr, (socklen_t)sizeof(addr));

    jarvis::SetNonBlock(fd);

    char buf[14] = {0};
    const char *msg = "jarvisye";
    buf[0] = 'y', buf[1] = 'e';
    uint32_t sz = htonl(14);
    printf("size: %u\n", sz);
    memcpy(buf + 2, (char*)&sz, 4);
    memcpy(buf + 6, msg, 8);
    printf("send msg: ");
    for (int i = 0; i < 10; i++)
    {
        printf("%d ", buf[i]);
    }
    printf("\n");

    assert(send(fd, buf, 14, 0) == 14);
    char echo[64];
    while ((ret = recv(fd, echo, 63, 0)) <= 0)
    {}
    
    if (ret > 0)
    {
        echo[ret] = '\0';
        printf("echo from server: %s\n", echo + 6);
    }
    else
    {
        printf("server error\n");
    }
    close(fd);
    return 0;
}