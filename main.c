#include "common.h"
#include "MySocket.h"
#include "command/server.h"
#include "command/client.h"
#include "TcpServer.h"
#include "Eventloop.h"
#include "Frame.h"
#include "Logger.h"
#include "ThreadFlag.h"

/**
 * server: ./ft [port]
 * e.g.    ./ft [5188]
 * 
 * client: ./ft <ip> <port>
 * e.g.    ./ft 127.0.0.1
 */

int main(int argc, char *argv[])
{
    // set log level
    setLogLevel(TRACE);

    int socketfd = -1;  // -1 for unconnected notation

    // server mode
    if(argc <= 2)
    {
        isClient = 0;
        // TODO: validate argv[1] is port
        int port = argv[1] ? atoi(argv[1]) : 5188;

        socketfd = initTcpServer("0.0.0.0", port);
        server_event_loop(socketfd);
    }

    // client mode
    else if (argc > 2)
    {
        isClient = 1;
        // TODO: validate argv
        const char *ip = argv[2] ? argv[1] : "127.0.0.1";
        int port = argv[1] ? atoi(argv[2]) : 5188;

        socketfd = initTcpClient(ip, port);

        // enable noblocking input
        // setNonblock(STDIN_FILENO, ON);

        showOpt();

        // worker thread get user input
        pthread_t workThread;
        pthread_create(&workThread, NULL, getOptInBackground, NULL);
        pthread_detach(workThread);

        client_event_loop(socketfd);
    }

    else
    {
        printf("please specify receiver's <port> [ip]\n");
        return -1;
    }

    return 0;
}