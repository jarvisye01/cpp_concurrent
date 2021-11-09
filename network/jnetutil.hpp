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

int Resolve(const char *host, const char *service, sockaddr_in *addr);

int SetSocketReuse(int fd);

int SetNonBlock(int fd);

}  // namespace jarvis

#endif