#ifndef MY_SOCKET_H__
#define MY_SOCKET_H__

#include <netinet/tcp.h>
#include <netinet/in.h>
#include "common.h"
#include "Logger.h"

typedef struct 
{
    int connectionFd;
    struct sockaddr_in address;
    socklen_t peerlen;
}SOCKET_PEER;

int socketfd(void);

struct sockaddr_in *requestAddress(const char *ip, int port);

void freeAddress(struct sockaddr_in * servaddr);

void BindAddress(int listenfd, struct sockaddr_in * servaddr);

void Listen(int listenfd);

void Accept(int listenfd, SOCKET_PEER *peeraddr);

void Connect(int listenfd, struct sockaddr_in * servaddr);

void setTcpNoDelay(int listenfd, OPT optval);

void setReuseAddr(int listenfd, OPT optval);

void setReusePort(int listenfd, OPT optval);

void setKeepAlive(int listenfd, OPT optval);

void setNonblock(int fd, OPT optval);

#endif