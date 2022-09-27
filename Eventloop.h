#ifndef EVENT_LOOP_H__
#define EVENT_LOOP_H__

#include "common.h"
#include "MySocket.h"
#include "TcpServer.h"
#include "Logger.h"

void server_event_loop(int listenfd);

void client_event_loop(int listenfd);

#endif