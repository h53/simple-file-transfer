#include "dispatcher.h"

void dispatchOpt(int opt, int socketfd)
{
    switch (opt)
    {
    case LS:
    {
        char buf[512];
        memset(buf, 0, sizeof(buf));
        printf("please input directory: ");
        scanf("%s", buf); // do mention that this way has injection hack
        // TODO: fix this method implementation
        do_ls(socketfd, buf);
        break;
    }

    case LC:
    {
        char buf[512];
        memset(buf, 0, sizeof(buf));
        printf("please input directory: ");
        scanf("%s", buf);
        do_lc(buf);
        showOpt();
        break;
    }

    case U:
    {
        char buf[512];
        memset(buf, 0, sizeof(buf));
        printf("please input file name: ");
        scanf("%s", buf);
        do_upload(socketfd, buf);
        break;
    }

    case D:
    {
        char buf[512];
        memset(buf, 0, sizeof(buf));
        printf("please input file name: ");
        scanf("%s", buf);
        do_download(socketfd, buf);
        break;
    }

    case S:
        do_suspend(socketfd);
        break;

    case C:
        do_continue(socketfd);
        break;

    case Q:
        do_quit(socketfd);
        break;

    default:
        break;
    }
}

void dispatchMetaData(MetaData *metaData, int socketfd)
{
    LOG_TRACE("dispatchMetaData");

    char cmd = metaData->cmd;

    switch (abs(cmd))
    {
    case LS:
        cmd > 0 ? do_server_ls(socketfd, metaData->data) : receive_server_ls(socketfd, metaData->data);
        break;

    case U:
        cmd > 0 ? do_server_upload(socketfd, metaData) : receive_server_upload(socketfd, metaData);
        break;

    case D:
        cmd > 0 ? do_server_download(socketfd, metaData) : receive_server_download(socketfd, metaData);
        break;

    case S:
        cmd > 0 ? do_server_suspend(socketfd, metaData) : receive_server_suspend(socketfd, metaData);
        break;

    case C:
        cmd > 0 ? do_server_continue(socketfd, metaData) : receive_server_continue(socketfd, metaData);
        break;

    case Q:
        cmd > 0 ? do_server_quit(socketfd, metaData) : receive_server_quit(socketfd, metaData);
        break;

        // never happen
    case LC:
    default:
        LOG_WARN("invalid mata data");
        break;
    }
}

// TODO: strategy mode
int getOpt(void)
{
    char inputbuf[1024];
    for (;;)
    //while (fgets(inputbuf, sizeof(inputbuf), stdin) != NULL)
    {
        scanf("%s", inputbuf);
        //TODO: make a table is more convenient
        if (0 == strncmp("ls", inputbuf, sizeof("ls")))
        {
            return LS;
        }
        else if (0 == strncmp("lc", inputbuf, sizeof("lc")))
        {
            return LC;
        }
        else if (0 == strncmp("u", inputbuf, sizeof("u")))
        {
            return U;
        }
        else if (0 == strncmp("d", inputbuf, sizeof("d")))
        {
            return D;
        }
        else if (0 == strncmp("s", inputbuf, sizeof("s")))
        {
            return S;
        }
        else if (0 == strncmp("c", inputbuf, sizeof("c")))
        {
            return C;
        }
        else if (0 == strncmp("q", inputbuf, sizeof("q")))
        {
            return Q;
        }
        else if (0 == strncmp("m", inputbuf, sizeof("m")))
        {
          printf("set debug level ?[1- 6] default to 5\n");
          scanf("%s", inputbuf);
          if(inputbuf[0] > '6' || inputbuf[0] < '1')
          {
            printf("invalid log level\n");
          }
          else
          {
            setLogLevel(inputbuf[0] - '1');
          }
        }
        else
        {
            printf("invalid input\n");
            showOpt();
        }
        memset(inputbuf, 0, sizeof(inputbuf));
    }
    return 0;
}

void showOpt(void)
{
    printf("===============================\n"
           "ls - look up server directory\n"
           "lc - look up client directory\n"
           "u  - upload file to server\n"
           "d  - download file to local\n"
           "s  - suspend transfer\n"
           "c  - continue suspended file\n"
           "q  - quit transfer\n"
           "m  - more options\n"
           "===============================\n");
}

void interactive(int socketfd, int opt, void (*callback)(void))
{
    dispatchOpt(opt, socketfd);
    callback();
}