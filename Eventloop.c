#include "Eventloop.h"

void server_event_loop(int listenfd)
{
    pid_t pid;
    for (;;)
    {
        SOCKET_PEER peeraddr;
        Accept(listenfd, &peeraddr); // connection data stored in peeraddr

        //set non block after accept
        setNonblock(peeraddr.connectionFd, ON);

        // accept success
        LOG_INFO("accept %s %d", inet_ntoa(peeraddr.address.sin_addr),
                 ntohs(peeraddr.address.sin_port));

        // create new process to handle connection
        pid = fork();
        if (pid == -1)
        {
            ERROR_EXIT("fork");
        }
        if (pid == 0) // child process
        {
            close(listenfd); // child process do not listen
            for (;;)         // blocking here
            {
                do_recv(peeraddr.connectionFd, &g_recvbuf);
            }
        }
        else
        {
            // father process do not handle connection
            close(peeraddr.connectionFd);
        }
    }
}

void client_event_loop(int listenfd)
{
    for (;;)
    {
        interactive(listenfd, getOptNotify(), resetOptNotify);
        do_recv(listenfd, &g_recvbuf);
    }
}
