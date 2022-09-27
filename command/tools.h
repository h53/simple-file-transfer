#ifndef TOOLS_H__
#define TOOLS_H__

#include "../common.h"
#include "../Logger.h"
#include "../Frame.h"
#include "../TcpServer.h"
#include "../ThreadFlag.h"

extern char g_current_transmit_file[1024];

char *prepareDataBuf(const char *buf, int size);

void releaseDataBuf(char *buf);

void transmitFileFrame(int socketfd, int filefd, char *buf, uint64_t seq, uint64_t sum);

int isTmpExists(int socketfd, const char *fileName);

void suspendTransfer(OPT on);

int isTransferSuspend(void);

void showDirectory(int output, const char *path);

void transmitFile(int socketfd, const char *fileName, char cmd);

const char *parseFileName(const char *filename);

void fileReceiver(int socketfd, MetaData *metaData);

void resetCurser(int fd);

uint64_t getFileSize(int fd);

int getPackages(int sum, int packageSize);

#endif