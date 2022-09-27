/**
 * @author: huangwushan
 * @date: 2021.7.9
 */
#ifndef _MY_COMMON_H
#define _MY_COMMON_H

#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h> /* atoi */
#include <stdio.h>  /* perror */
#include <string.h> /* memset */
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* write */
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h> /* byte order convert */
#include <assert.h>
#include <pthread.h>

#define ERROR_EXIT(m)       \
    do                      \
    {                       \
        perror(m);          \
        exit(EXIT_FAILURE); \
    } while (0)

// TODO: prettier here
#define LS 1
#define LC 2
#define U 3
#define D 4
#define S 5
#define C 6
#define Q 7
#define H 8

typedef enum
{
    OFF = 0,
    ON = 1
} OPT;

typedef struct
{
    int len;
    char *data;
} Frame;

// typedef enum
// {
//     LS = 1,
//     LC,
//     U,
//     D,
//     S,
//     C,
//     Q,
//     H
// }CMD;

extern int isClient;

#endif