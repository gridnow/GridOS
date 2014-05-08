#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>

int socket(int domain, int type, int protocol);
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

#endif
