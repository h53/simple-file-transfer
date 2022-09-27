#include "client.h"

void do_lc(const char *path)
{
    LOG_TRACE("do_lc");
    showDirectory(STDOUT_FILENO, path);
}

void do_ls(int socketfd, const char *path)
{
    LOG_TRACE("do_ls");
    do_send(socketfd, LS, 0, 0, path);
}

void do_upload(int socketfd, const char *fileName)
{
    LOG_TRACE("do_upload");
    transmitFile(socketfd, fileName, U);
}

void do_download(int socketfd, const char *filename)
{
    LOG_TRACE("do_download");
    do_send(socketfd, D, 0, 0, filename);
}

void do_suspend(int socketfd)
{
    LOG_TRACE("do_suspend");
    suspendTransfer(ON);
    do_send(socketfd, S, 0, 0, NULL);
}

void do_continue(int socketfd)
{
    LOG_TRACE("do_continue");
    suspendTransfer(OFF);
    do_send(socketfd, C, 0, 0, NULL);
}

void do_quit(int socketfd)
{
    LOG_TRACE("do_quit");
    do_send(socketfd, Q, 0, 0, NULL);

    //TODO: exit gracefully
    exit(EXIT_SUCCESS);
}

// receive reply from server
void receive_server_lc(int socketfd, const char *data)
{
    LOG_FATAL("no implementation");
}

void receive_server_ls(int socketfd, const char *data)
{
    LOG_TRACE("receive_server_ls");
    printf("%s", data);
    showOpt();
}

void receive_server_upload(int socketfd, MetaData *metaData)
{
    LOG_TRACE("receive_server_upload");
    const int packageSize = 1024;
    char buf[packageSize];
    uint64_t seq = 1;

    static int fd = -1;

    // only open once
    if (fd < 0)
    {
        fd = open(g_current_transmit_file, O_RDWR, 0666);
        if (fd < 0)
        {
            ERROR_EXIT("open");
        }
    }

    uint64_t sum = getPackages(getFileSize(fd), packageSize);

    // exists tmp file in server
    if (1 == metaData->slot)
    {
        LOG_DEBUG("g_current_transmit_file %s", g_current_transmit_file);

        char *md5buf = malloc(metaData->param);
        int ret = read(fd, md5buf, metaData->param);
        
        //assert(ret == metaData->param);
        LOG_DEBUG("read %d", ret);

        uint8_t hash[16];
        memset(hash, 0, sizeof(hash));
        md5(md5buf, metaData->param, hash);

        if (hashEqual(hash, metaData->data))
        {
            seq += metaData->param / packageSize;
        }
        else
        {
            resetCurser(fd);
        }
        free(md5buf);
    }
    LOG_DEBUG("transFileFrame seq = %ld total package = %ld", seq, sum);
    transmitFileFrame(socketfd, fd, buf, seq, sum);
    // clean work
    memset(g_current_transmit_file, 0, sizeof(g_current_transmit_file));
    close(fd);
    fd = -1;

    if(isClient)    // client
    {
        showOpt();
    }
}

void receive_server_download(int socketfd, MetaData *metaData)
{
    LOG_TRACE("receive_server_download");
    fileReceiver(socketfd, metaData);
}

void receive_server_suspend(int socketfd, MetaData *metaData)
{
    LOG_TRACE("receive_server_suspend");
}

void receive_server_continue(int socketfd, MetaData *metaData)
{
    LOG_TRACE("receive_server_continue");
}

void receive_server_quit(int socketfd, MetaData *metaData)
{
    LOG_TRACE("receive_server_quit");
}