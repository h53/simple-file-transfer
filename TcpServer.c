#include "TcpServer.h"

RecvBuf g_recvbuf;

static int getBufRemainSpace(RecvBuf *recvbuf)
{
    return sizeof(recvbuf->buf) - (recvbuf->write_index - recvbuf->buf);
}

static int getBufRemainData(RecvBuf *recvbuf)
{
    return recvbuf->write_index - recvbuf->read_index;
}

static void moveBuf(char *buf, int remain, int len)
{
    LOG_TRACE("moveBuf remain %d, len %d", remain, len);
    memcpy(buf, buf + len - remain, len);
}

void initRecvBuf(RecvBuf *recvbuf)
{
    //reset offset
    recvbuf->write_index = recvbuf->buf;
    recvbuf->read_index = recvbuf->buf;
    recvbuf->len = 0;
}

static void receiveData(int socketfd, RecvBuf *recvbuf)
{
    LOG_TRACE("receiveData");
    int remainData = getBufRemainData(recvbuf);
    LOG_DEBUG("remain data is %d", remainData);
    // TODO: fix magic number here 6 = 1 + 1 + 8
    if (remainData >= getFrameHeadSize())
    {
        MetaData *metaData = parseFrameHead(recvbuf->read_index);
        int frameDataSize = getFrameHeadSize() + getFrameDataSize(metaData->cmd, metaData->param);
        LOG_DEBUG("frameDataSize is %d", frameDataSize);
        if (remainData >= frameDataSize)
        {
            metaData->data = parseFrameData(metaData->cmd,
                                            metaData->param,
                                            recvbuf->read_index);

            LOG_INFO("recv msg: %d %d %ld src is below \n"
                     "------------------------------------------\n"
                     "%s\n"
                     "------------------------------------------",
                     metaData->cmd, metaData->slot, metaData->param, metaData->data);

            dispatchMetaData(metaData, socketfd);
            releaseMetaData(metaData);

            // clear haveread buf
            //memset(recvbuf->read_index, 0, frameDataSize);

            recvbuf->read_index += frameDataSize;
            LOG_DEBUG("!remain data is %d", getBufRemainData(recvbuf));
            receiveData(socketfd, recvbuf);
        }
    }
}

int initTcpServer(const char *ip, int port)
{
    LOG_TRACE("init server %s %d", ip, port);

    int listenfd = socketfd();
    struct sockaddr_in *servaddr = requestAddress(ip, port);

    BindAddress(listenfd, servaddr);

    // set tcp option
    setReuseAddr(listenfd, ON);
    setReusePort(listenfd, ON);
    setTcpNoDelay(listenfd, OFF);
    setKeepAlive(listenfd, ON);

    // initialize global recvbuf
    initRecvBuf(&g_recvbuf);

    Listen(listenfd);

    printf("server runing on port: %d\n", port);

    return listenfd;
}

int initTcpClient(const char *ip, int port)
{
    LOG_TRACE("init client %s %d", ip, port);

    int listenfd = socketfd();
    struct sockaddr_in *servaddr = requestAddress(ip, port);

    // BindAddress(listenfd, servaddr); // client no need to bind

    // set tcp option
    setReuseAddr(listenfd, ON);
    setReusePort(listenfd, ON);
    setTcpNoDelay(listenfd, OFF);
    setKeepAlive(listenfd, ON);

    // initialize global recvbuf
    initRecvBuf(&g_recvbuf);

    Connect(listenfd, servaddr);

    //set non block after connected
    setNonblock(listenfd, ON);

    return listenfd;
}

void do_recv(int conn, RecvBuf *recvbuf)
{
    //LOG_TRACE("do_recv");

    // non block mode
    // memset(recvbuf, 0, sizeof(recvbuf));
    recvbuf->len = read(conn, recvbuf->write_index, getBufRemainSpace(recvbuf));
    if (recvbuf->len == 0)
    {
        if (getBufRemainSpace(recvbuf) == 0)
        {
            //LOG_DEBUG("buffer is full");
        }
        else
        {
            LOG_WARN("connection closed %d", conn);
            exit(EXIT_FAILURE);
            // break;
        }
    }
    else if (recvbuf->len == -1)
    {
        if (errno == EINTR || errno == EAGAIN)
        {
            // read again later
            recvbuf->len = 0; // reset buffer offset
        }
        else
        {
            ERROR_EXIT("read");
        }
    }
    else
    {
        recvbuf->write_index += recvbuf->len;

        LOG_DEBUG("recv_len = %d write_index = %ld read_index = %ld recvbuf.remain = %d", recvbuf->len,
                  recvbuf->write_index - recvbuf->buf, recvbuf->read_index - recvbuf->buf, getBufRemainSpace(recvbuf));

#ifdef DEBUG_PROCESS
        // test process
        for (;;)
        {
            LOG_DEBUG("%s", g_recvbuf);
        }
#endif
        LOG_INFO("recv %d bytes", recvbuf->len);
        receiveData(conn, recvbuf);

        LOG_DEBUG("recv_len = %d write_index = %ld read_index = %ld recvbuf.remain = %d", recvbuf->len,
                  recvbuf->write_index - recvbuf->buf, recvbuf->read_index - recvbuf->buf, getBufRemainSpace(recvbuf));

        // check and move buf
        if ((recvbuf->write_index - recvbuf->read_index < 8 && recvbuf->read_index != recvbuf->buf) || getBufRemainSpace(recvbuf) < 2048)
        {
            LOG_DEBUG("move buf");
            //moveBuf(recvbuf.buf, recvbuf.remain, recvbuf.write_index - recvbuf.read_index);
            memcpy(recvbuf->buf, recvbuf->read_index, recvbuf->write_index - recvbuf->read_index);
            //resetRecvBuf(&recvbuf);
            recvbuf->write_index = recvbuf->buf + (recvbuf->write_index - recvbuf->read_index);
            recvbuf->read_index = recvbuf->buf;

            LOG_DEBUG("recv_len = %d write_index = %ld read_index = %ld remain_space = %d", recvbuf->len,
                      recvbuf->write_index - recvbuf->buf, recvbuf->read_index - recvbuf->buf, getBufRemainSpace(recvbuf));
        }
    }
}

void do_send(int conn, char cmd, char slot,
             uint64_t param, const char *data)
{
    if (socketfd < 0)
    {
        LOG_FATAL("no server available!");
        return;
    }
    Frame *frame = prepareFrame(cmd, slot, param, data);

    LOG_INFO("Frame data strlen = %ld , size = %d", strlen(frame->data), frame->len);
#if 1
    // TODO: simplify this repeat code
    MetaData *metaData = parseFrameHead(frame->data);
    metaData->data = parseFrameData(metaData->cmd,
                                    metaData->param,
                                    frame->data);
    LOG_INFO("do_send: %d %d %ld %s", metaData->cmd, metaData->slot, metaData->param, metaData->data);
    releaseMetaData(metaData);
#endif
    sendFrame(conn, frame);
    releaseFrame(frame);
}