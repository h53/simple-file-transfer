#ifndef CLIENT_H__
#define CLIENT_H__

#include "../common.h"
#include "tools.h"
#include "../3rd/md5.h"

void do_lc(const char *path);

void do_ls(int socketfd, const char *path);

void do_upload(int socketfd, const char *fileName);

void do_download(int socketfd, const char *filename);

void do_suspend(int socketfd);

void do_continue(int socketfd);

void do_quit(int socketfd);

// receive reply from server
void receive_server_lc(int socketfd, const char *data);

void receive_server_ls(int socketfd, const char *data);

void receive_server_upload(int socketfd, MetaData *metaData);

void receive_server_download(int socketfd, MetaData *metaData);

void receive_server_suspend(int socketfd, MetaData *metaData);

void receive_server_continue(int socketfd, MetaData *metaData);

void receive_server_quit(int socketfd, MetaData *metaData);

#endif
