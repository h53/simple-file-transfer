#ifndef DISPATCHER_H__
#define DISPATCHER_H__

#include "common.h"
#include "Frame.h"
#include "command/client.h"
#include "command/server.h"

void dispatchMetaData(MetaData *metaData, int socketfd);

void dispatchOpt(int opt, int socketfd);

int getOpt(void);

void showOpt(void);

void interactive(int socketfd, int opt, void (*callback)(void));

#endif