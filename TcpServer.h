#ifndef TCP_SERVER_H__
#define TCP_SERVER_H__

#include "common.h"
#include "MySocket.h"
#include "Logger.h"
#include "Frame.h"
#include "command/server.h"
#include "command/client.h"
#include "dispatcher.h"

typedef struct
{
    char buf[8192];
    // offset
    char *write_index;
    char *read_index;
    int len;
} RecvBuf;

extern RecvBuf g_recvbuf;

int initTcpServer(const char *ip, int port);

int initTcpClient(const char *ip, int port);

void do_recv(int conn, RecvBuf *recvbuf);

void do_send(int conn, char cmd, char slot,
             uint64_t param, const char *data);

#endif