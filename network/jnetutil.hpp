#ifndef J_NET_H
#define J_NET_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <inttypes.h>

namespace jarvis
{

const int JNET_ERR = -1;
const int JNET_SUCC = 0;

int Resolve(char *host, char *service, struct addrinfo *hint, sockaddr_in *addr);

}  // namespace jarvis

#endif