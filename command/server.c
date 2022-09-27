#include "server.h"

void do_server_lc(int socketfd, const char *path)
{
    LOG_FATAL("no implementation");
}

void do_server_ls(int socketfd, const char *path)
{
    LOG_TRACE("do_server_ls");
    showDirectory(socketfd, path);
}

void do_server_upload(int socketfd, MetaData *metaData)
{
    LOG_TRACE("do_server_upload");
    fileReceiver(socketfd, metaData);
}

void do_server_download(int socketfd, MetaData *metaData)
{
    LOG_TRACE("do_server_download");
    transmitFile(socketfd, metaData->data, -metaData->cmd);
}

void do_server_suspend(int socketfd, MetaData *metaData)
{
    LOG_TRACE("suspendTransfer ON");
    suspendTransfer(ON);
}

void do_server_continue(int socketfd, MetaData *metaData)
{
    LOG_TRACE("suspendTransfer OFF");
    suspendTransfer(OFF);
}

void do_server_quit(int socketfd, MetaData *metaData)
{
    printf("client quit, this session closed\n");
    //TODO: exit gracefully
    exit(EXIT_SUCCESS);
}