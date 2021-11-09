#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

int SetNoBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(int argc, char ** argv)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("client socket %d\n", fd);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);
    addr.sin_port = htons(9000);

    printf("addr: %s\n", inet_ntoa(addr.sin_addr));
    printf("port: %d\n", 9000);
    
    socklen_t len = sizeof(addr);
    int ret = connect(fd, (sockaddr*)&addr, len);

    SetNoBlock(fd);

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