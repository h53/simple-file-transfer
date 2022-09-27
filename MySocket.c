#include "MySocket.h"

int socketfd(void)
{
    int sock;
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        ERROR_EXIT("socket");
    }
    
    LOG_TRACE("get socket fd %d", sock);
    return sock;
}

struct sockaddr_in *requestAddress(const char *ip, int port)
{
    struct sockaddr_in *servaddr = malloc(sizeof(struct sockaddr_in));
    memset(servaddr, 0, sizeof(*servaddr));
    servaddr->sin_family = AF_INET;
    servaddr->sin_port = htons(port);
    servaddr->sin_addr.s_addr = inet_addr(ip);

    LOG_TRACE("request Address");
    return servaddr;
}

void freeAddress(struct sockaddr_in *servaddr)
{
    free(servaddr);
    LOG_TRACE("free Address");
}

void BindAddress(int listenfd, struct sockaddr_in *servaddr)
{
    if (bind(listenfd, (struct sockaddr *)servaddr, sizeof(*servaddr)) < 0)
    {
        ERROR_EXIT("bind");
    }
    LOG_TRACE("Bind");
}

void Listen(int listenfd)
{
    if (listen(listenfd, SOMAXCONN) < 0)
    {
        ERROR_EXIT("listen");
    }
    LOG_TRACE("Listen");
}

void Accept(int listenfd, SOCKET_PEER *peeraddr)
{
    peeraddr->connectionFd = accept(listenfd, (struct sockaddr *)&(peeraddr->address),
                                             &(peeraddr->peerlen));
    if (peeraddr->connectionFd == -1)
    {
        ERROR_EXIT("accept");
    }
    LOG_TRACE("Accept");
}

void Connect(int listenfd, struct sockaddr_in * servaddr)
{
    if (connect(listenfd, (struct sockaddr *)servaddr, sizeof(*servaddr)) < 0)
    {
        ERROR_EXIT("connect");
    }
    LOG_TRACE("Connect");
}

void setTcpNoDelay(int listenfd, OPT optval)
{
    int on = (int)optval;
    if (setsockopt(listenfd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) < 0)
    {
        ERROR_EXIT("setsocketopt");
    }
    LOG_TRACE("TCP NO DELAY %d", on);
}

void setReuseAddr(int listenfd, OPT optval)
{
    int on = (int)optval;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        ERROR_EXIT("setsocketopt");
    }
    LOG_TRACE("REUSE ADDR %d", on);
}

void setReusePort(int listenfd, OPT optval)
{
    int on = (int)optval;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) < 0)
    {
        ERROR_EXIT("setsocketopt");
    }
    LOG_TRACE("REUSE PORT %d", on);
}

void setKeepAlive(int listenfd, OPT optval)
{
    int on = (int)optval;
    if (setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on)) < 0)
    {
        ERROR_EXIT("setsocketopt");
    }
    LOG_TRACE("Keep Alive %d", on);
}

void setNonblock(int fd, OPT optval)
{
    int flags = fcntl(fd, F_GETFL);
    if(flags == -1)
    {
        ERROR_EXIT("fcntl");
    }

    if(optval == ON)
    {
        flags |= O_NONBLOCK;
    }
    else
    {
        flags &= ~O_NONBLOCK;
    }
    int ret = fcntl(fd, F_SETFL, flags);
    if(ret == -1)
    {
        ERROR_EXIT("fcntl");
    }
}