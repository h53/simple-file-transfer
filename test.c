/**
 * get idea from https://zhuanlan.zhihu.com/json-tutorial
 */
#include "common.h"
#include "MySocket.h"
#include "Frame.h"
#include "Eventloop.h"
#include "TcpServer.h"
#include "command/server.h"
#include "command/client.h"
#include "Logger.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")
#define EXPECT_EQ_STR(expect, actual, len) EXPECT_EQ_BASE(0 == strncmp((expect),(actual),(len)), expect, actual, "%s")

static void test_1() {
    EXPECT_EQ_INT(1,1); //test example;
}

static void test_Frame(char cmd, char slot,
                          uint64_t param, const char *data, int len)
{
    Frame *frame = prepareFrame(cmd,slot,param,data);
    MetaData *metaData = parseFrameHead(frame->data);
    metaData->data = parseFrameData(metaData->cmd,
                                    metaData->param,
                                    frame->data);

    EXPECT_EQ_INT(len,frame->len);

    EXPECT_EQ_INT(cmd,frame->data[0]);
    EXPECT_EQ_INT(cmd,metaData->cmd);

    EXPECT_EQ_INT(slot,frame->data[1]);
    EXPECT_EQ_INT(slot,metaData->slot);

    if(param)
    {
        EXPECT_EQ_INT(param,ntohl(*(uint32_t *)&frame->data[2]));
        EXPECT_EQ_INT(param, (uint32_t)metaData->param);
    }
    else
    {
        EXPECT_EQ_INT(frame->len,ntohl(*(uint32_t *)&frame->data[2]));
        EXPECT_EQ_INT(len,(uint32_t)metaData->param);
    }

    if(data)
    {
        EXPECT_EQ_STR(frame->data, data, strlen(data));
        EXPECT_EQ_STR(data, metaData->data, strlen(data));
    }
    else
    {
        // no need to test, because buf has no data field here
        //EXPECT_EQ_INT(NULL, metaData->data);
    }
    releaseFrame(frame);
    releaseMetaData(metaData);
}

// static void test_send() {
//     pid_t pid = fork();
//     if(-1 == pid)
//     {
//         ERROR_EXIT("fork");
//     }
//     if(0 == pid)    //child process
//     {
//         int socketfd = initTcpClient("127.0.0.1",5188);
//         Frame* frame = prepareFrame(LS, 0, 0, ".");
//         sendFrame(socketfd, frame);
//         releaseFrame(frame);
//     }
//     else
//     {
//         int socketfd = initTcpServer("0.0.0.0", 5188);
//         for(;;)
//         {
//             SOCKET_PEER peeraddr;
//             Accept(socketfd, &peeraddr);
//             do_recv(peeraddr.connectionFd, );
//         }
//     }
    
// }

static void test() {
    test_1();
    /* ... */
    test_Frame(LS, 0, 0, "", 10);
    // test_Frame(LS, 0, 32, "hello", 11);
    // test_Frame(LS, 0, 32, NULL, 6);
    test_Frame(LC, 0, 0, NULL, 6);
    test_Frame(LS, 0, 0, ".", 7);
    test_Frame(LS, 0, 0, "yes i do", 14);
    test_Frame(-LS, 0, 0, "unb239胡说", 18);

    // test send
    /* test_send(); */

    //test socket
}

int main() {
    setLogLevel(TRACE);
    test();

    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}