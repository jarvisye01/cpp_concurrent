#include <sys/socket.h>
#include <fcntl.h>
#include "network/jnetutil.hpp"

namespace jarvis
{

int Resolve(const char *host, const char *service, sockaddr_in *addr)
{
    struct addrinfo hint, *result = NULL;
    hint.ai_family = AF_INET;
    hint.ai_protocol = 	IPPROTO_IP;
    if ((getaddrinfo(host, service, NULL, &result)) != 0)
        return JNET_ERR;
    if (result == NULL)
        return JNET_ERR;
    for (auto ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        // only ipv4
        if (ptr->ai_protocol != IPPROTO_IP)
            continue;
        if (ptr->ai_addr != NULL)
        {
            sockaddr_in *p = (sockaddr_in*)(ptr->ai_addr);
            addr->sin_family = p->sin_family;
            addr->sin_port = p->sin_port;
            addr->sin_addr = p->sin_addr;
            break;
        }
    }
    if (result != NULL)
        freeaddrinfo(result);
    return JNET_SUCC;
}

int SetSocketReuse(int fd)
{
    int op = 1;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*)&op, sizeof(int));
}

int SetNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

}; // namespace jarvis