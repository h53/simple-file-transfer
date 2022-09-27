#ifndef SERVER_H__
#define SERVER_H__

#include "../common.h"
#include "../Logger.h"
#include "tools.h"

void do_server_lc(int socketfd, const char *path);

void do_server_ls(int socketfd, const char *path);

void do_server_upload(int socketfd, MetaData *metaData);

void do_server_download(int socketfd, MetaData *metaData);

void do_server_suspend(int socketfd, MetaData *metaData);

void do_server_continue(int socketfd, MetaData *metaData);

void do_server_quit(int socketfd, MetaData *metaData);

#endif
