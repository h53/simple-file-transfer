#ifndef FRAME_H__
#define FRAME_H__

#include "common.h"
#include "Logger.h"
#include "EndianConvert.h"

typedef struct 
{
    char cmd;
    char slot;
    uint64_t param;
    char *data;
}MetaData;

char *parseFrameData(char cmd, uint64_t param, char *src);

int getFrameDataSize(char cmd, uint64_t param);

int getFrameHeadSize(void);

MetaData *parseFrameHead(const char *buf);

Frame *prepareFrame(char cmd, char slot,
                    uint64_t param, const char *data);

void sendFrame(int conn, Frame *frame);

void releaseFrame(Frame *frame);

void releaseMetaData(MetaData *metaData);

#endif