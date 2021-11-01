#include "network/jnetutil.hpp"

namespace jarvis
{

int Resolve(char *host, char *service, struct addrinfo *hint, sockaddr_in *addr)
{
    struct addrinfo *result;
    int ret;
    if ((ret = getaddrinfo(host, service, hint, &result)) != 0)
        return JNET_ERR;
    if (result == NULL)
        return JNET_ERR;
    for (auto ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        // only ipv4
        if (ptr->ai_protocol  != IPPROTO_IP)
            continue;
        sockaddr_in *p = static_cast<sockaddr_in*>(ptr->ai_addr);
        addr->sin_family = p->sin_family;
        addr->sin_port = p->sin_port;
        addr->sin_addr = p->sin_addr;
        break;
    }
    freeaddrifo(result);
    return JNET_SUCC;
}

}; // jarvis