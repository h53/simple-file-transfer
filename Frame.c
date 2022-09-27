#include "Frame.h"

Frame *prepareFrame(char cmd, char slot,
                    uint64_t param, const char *data)
{
    LOG_TRACE("prepareFrame %d %d %ld\n"
              "------------------------------------------\n"
              "%s\n"
              "------------------------------------------",
              cmd, slot, param, data);

    Frame *frame = malloc(sizeof(Frame));
    memset(frame, 0, sizeof(Frame));

    if (!param) // if param is zero, we put length here
    {
        param = sizeof(cmd) + sizeof(slot) +
                sizeof(param) + (data ? strlen(data) : 0);
    }

    int dataSize = getFrameDataSize(cmd, param);

    frame->len = sizeof(cmd) + sizeof(slot) +
                 sizeof(param) + dataSize;

    frame->data = malloc(frame->len);
    memset(frame->data, 0, frame->len);

    LOG_TRACE("frame data size is %d", dataSize);

    char *index = frame->data;
    *index++ = cmd;

    *index++ = slot;

    // LOG_DEBUG("cmd = %d slot = %d, len = %d, param = %d", cmd, slot, frame->len, param);
    // LOG_DEBUG("data[0] = %d, data[1] = %d", frame->data[0], frame->data[1]);

    uint64_t netParam = htobe64(param);
    memcpy(index, &netParam, sizeof(param)); // convert to net order

    //null pointer check
    if (data)
    {
        memcpy(index + sizeof(param), data, dataSize); 
    }
    // LOG_DEBUG("data[0] = %d, data[1] = %d", frame->data[0], frame->data[1]);

    return frame;
}

int getFrameDataSize(char cmd, uint64_t param)
{
    int size = 0;
    switch (abs(cmd))
    {
    case LS:
    case S:
    case Q:
        size = param - sizeof(cmd) * 2 - sizeof(param);
        break;

    case U:
    case D:
    case C:
        size = 1024;
        break;

    case LC:
    default:
        break;
    }

    return size;
}

int getFrameHeadSize(void)
{
    return sizeof(char) * 2 + sizeof(uint64_t);
}

char *parseFrameData(char cmd, uint64_t param, char *src)
{
    char *data = NULL;

    int size = getFrameDataSize(cmd, param);
    LOG_TRACE("getFrameDataSize = %d",size);

    if (size > 0)
    {
        data = malloc(size + 1);
        memset(data, 0, size + 1);
        memcpy(data, src + sizeof(cmd) * 2 + sizeof(param), size - sizeof(cmd) * 2 + sizeof(param));
        data[size] = '\0';
    }

    if (NULL == data)
    {
        LOG_WARN("parseFrameData is null");
    }

    return data;
}

MetaData *parseFrameHead(const char *buf)
{
    MetaData *metaData = malloc(sizeof(MetaData));
    metaData->cmd = buf[0];
    // TODO: fix here magic number
    if (metaData->cmd <= H && metaData->cmd >= LS || metaData->cmd >= -H && metaData->cmd <= -LS)
    {
        metaData->slot = buf[1];
        metaData->param = be64toh(*(uint64_t *)&buf[2]);

        metaData->data = NULL;
        return metaData;
    }
    // communication fail
    LOG_FATAL("Frame format is wrong cmd = %d slot = %d param = %ld", metaData->cmd, metaData->slot, metaData->param);
}

void sendFrame(int conn, Frame *frame)
{
    LOG_TRACE("sendFrame");
    if (!frame)
    {
        LOG_FATAL("frame is NULL");
    }

    int nsend = 0;
    int nleft = frame->len; // send false here
    while (nleft > 0)
    {
        if ((nsend = write(conn, frame->data + nsend, nleft)) < 0)
        {
            if (errno == EINTR || errno == EAGAIN)
            {
                nsend = 0;
                continue;
            }
            ERROR_EXIT("send");
        }
        else if (nsend == 0)
        {
            printf("connection closed!\n");
            continue;
        }

        LOG_INFO("sended %d bytes", nsend);
        nleft -= nsend;
    }
}

void releaseFrame(Frame *frame)
{
    free(frame->data);
    free(frame);
}

void releaseMetaData(MetaData *metaData)
{
    free(metaData->data);
    free(metaData);
}