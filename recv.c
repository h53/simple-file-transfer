// /**
//  * filetransfer [send|recv] [ip] <port> [file]
//  **/

// #include "common.h"

// #define ERROR_EXIT(m)       \
//     do                      \
//     {                       \
//         perror(m);          \
//         exit(EXIT_FAILURE); \
//     } while (0)

// int do_ls(int sockfd, char *path);

// void do_recv(int conn)
// {
//     char recvbuf[2048];
//     for (;;)
//     {
//         memset(recvbuf, 0, sizeof(recvbuf));
//         int ret = read(conn, recvbuf, sizeof(recvbuf));
//         if (ret == 0)
//         {
//             printf("\nclient closed\n");
//             break;
//         }
//         else if (ret == -1)
//         {
//             ERROR_EXIT("read");
//         }

//         // test process
// #ifdef DEBUG_PROCESS
//         for (;;)
//         {
//             printf("%s", recvbuf);
//         }
// #endif
//         fputs(recvbuf, stdout);
//     }
// }

// int main(int argc, char *argv[])
// {
//     do_ls(1,".");
//     // if (argc != 3)
//     // {
//     //     printf("please specify receiver's <ip> and <port>\n");
//     //     return -1;
//     // }
//     printf("init\n");

//     int port = atoi(argv[2]);
//     int listenfd;

//     if ((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
//     {
//         ERROR_EXIT("socket");
//     }

//     struct sockaddr_in servaddr;
//     memset(&servaddr, 0, sizeof(servaddr));
//     servaddr.sin_family = AF_INET;
//     servaddr.sin_port = htons(5188);
//     servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

//     // reuse addr
//     int on = 1;
//     if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
//     {
//         ERROR_EXIT("setsocketopt");
//     }

//     if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
//     {
//         ERROR_EXIT("bind");
//     }

//     if (listen(listenfd, SOMAXCONN) < 0)
//     {
//         ERROR_EXIT("listen");
//     }

//     // server run normally
//     printf("listening...\n");

//     pid_t pid;
//     for (;;)
//     {
//         struct sockaddr_in peeraddr;
//         socklen_t peerlen = sizeof(peeraddr);

//         int conn = accept(listenfd, (struct sockaddr *)&peeraddr, &peerlen);
//         if (conn == -1)
//         {
//             ERROR_EXIT("accept");
//         }

//         // accept success
//         printf("accept %s %d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));

//         // create new process to handle connection
//         pid = fork();
//         if (pid == -1)
//         {
//             ERROR_EXIT("fork");
//         }
//         if (pid == 0) // child process
//         {
//             close(listenfd); // child process do not listen
//             do_recv(conn);
//             printf("connection fd closed\n");
//             exit(EXIT_SUCCESS);
//         }
//         else
//         {
//             // father process do not handle connection
//             close(conn);
//         }
//     }
// }

// int do_ls(int sockfd, char *path)
// {
//     char cmd[64];
//     char buf[1024];
//     memset(buf, 0, sizeof(buf));
//     int fd;
//     int len;

//     sprintf(cmd, "ls %s > temp.txt", path);
//     fprintf(stdout, "===from client:(%s)\n", cmd);
//     system(cmd);

//     if ((fd = open("temp.txt", O_RDONLY)) < 0)
//     {
//         fprintf(stderr, "ERROR open");
//         return -1;
//     }

//     read(fd, buf, sizeof(buf));
//     printf("%s", buf);
// }