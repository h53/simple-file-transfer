#include "tools.h"

static int g_suspend = 0;
char g_current_transmit_file[1024] = {0};

void suspendTransfer(OPT on)
{
    g_suspend = on;
}

int isTransferSuspend(void)
{
    return g_suspend;
}

// TODO: use DIR to change implementation
void showDirectory(int output, const char *path)
{
    if (output < 0)
    {
        LOG_FATAL("bad output");
    }

    //TODO: validate path
    LOG_TRACE("show directory");
    char cmd[64] = {0};
    char buf[4096] = {0};
    memset(cmd, 0, sizeof(cmd));
    memset(buf, 0, sizeof(buf));
    int fileFd;

    sprintf(cmd, "ls %s > temp.txt", path);
    LOG_DEBUG("===from client:(%s)", cmd);
    system(cmd);

    if ((fileFd = open("temp.txt", O_RDONLY)) < 0)
    {
        ERROR_EXIT("open");
    }

    int ret;

    while ((ret = read(fileFd, buf, sizeof(buf))) > 0)
    {
        LOG_DEBUG("read");
        if (output == STDOUT_FILENO)
        {
            printf("%s", buf);
        }
        else
        {
            char *data = prepareDataBuf(buf, ret);
            do_send(output, -LS, 0, 0, data);
            releaseDataBuf(data);
        }
    }
    if (ret == 0)
    {
        LOG_TRACE("end of file");
    }
    else if (ret == -1)
    {
        ERROR_EXIT("read");
    }

    close(fileFd);
}

void transmitFile(int socketfd, const char *fileName, char cmd)
{
    const int packageSize = 1024;

    LOG_TRACE("transmit file");
    if (socketfd < 0)
    {
        LOG_FATAL("no server available!");
    }

    if (NULL == fileName)
    {
        LOG_WARN("no file name");
        return;
    }
    if (-1 == access(fileName, R_OK | W_OK | F_OK))
    {
        LOG_WARN("cant upload this file");
        printf("cant upload this file\n");

        return;
    }

    int fd = open(fileName, O_RDONLY);
    if (fd < 0)
    {
        ERROR_EXIT("open");
    }

    memset(g_current_transmit_file, 0, sizeof(g_current_transmit_file));
    memcpy(g_current_transmit_file, fileName, strlen(fileName));

    // calculate package num
    uint64_t size = getFileSize(fd);
    uint64_t sum = getPackages(size, packageSize);
    LOG_INFO("have %ld packages to send", sum);

    char buf[packageSize];
    uint64_t seq = 1;

    // send the 0 package
    // -----------------------------
    // | CMD | 1 | size | fileName |
    // -----------------------------
    do_send(socketfd, cmd, 1, size, fileName);

    close(fd);
}

uint64_t getFileSize(int fd)
{
    LOG_TRACE("getFileSize");
    uint64_t size = 0;

    resetCurser(fd);
    if (-1 == (size = lseek(fd, 0, SEEK_END)))
    {
        ERROR_EXIT("lseek end");
    }
    resetCurser(fd);
    LOG_INFO("file size = %ld", size);
    return size;
}

void resetCurser(int fd)
{
    if (-1 == lseek(fd, 0, SEEK_SET))
    {
        ERROR_EXIT("lseek reset");
    }
}

int getPackages(int sum, int packageSize)
{
    return (sum + packageSize - 1) / packageSize; // less than package size also send package size frame
}

const char *parseFileName(const char *filename)
{
    LOG_TRACE("parse fileName");
    for (int i = strlen(filename) - 1; i > 0; i--)
    {
        if (filename[i] == '/' || filename[i] == '\\')
        {
            return filename + i + 1;
        }
    }
    return filename;
}

void fileReceiver(int socketfd, MetaData *metaData)
{
    LOG_TRACE("fileReceiver");
    const int packageSize = 1024;

    static char *fileName = NULL;
    static char *plainName = NULL;
    static int64_t fileSize = 0;
    static int nameLength = 0;
    static int fd = 0;

    static uint64_t offset = 0; // already received in tmp file

    // receive the 0 package
    if (1 == metaData->slot)
    {
        const char *fileIndex = parseFileName(metaData->data);
        nameLength = strlen(fileIndex);
        plainName = malloc(nameLength + 1);
        memcpy(plainName, fileIndex, nameLength);
        plainName[nameLength] = '\0';

        fileName = malloc(nameLength + strlen(".tmp") + 1);
        memset(fileName, 0, nameLength + strlen(".tmp") + 1);
        memcpy(fileName, plainName, nameLength);
        memcpy(fileName + nameLength, ".tmp", strlen(".tmp"));

        fileSize = metaData->param;

        if (isTmpExists(socketfd, fileName))
        {
            // write to file
            if (fd > 0)
            {
                // reuse fd
            }
            else
            {
                fd = open(fileName, O_RDWR | O_CREAT, 0666);
                if (fd < 0)
                {
                    ERROR_EXIT("open");
                }
            }

            // calculate package num
            offset = getFileSize(fd);
            LOG_INFO("tmp file size is %ld", offset);

            char *buf = malloc(offset);
            int ret = read(fd, buf, offset);
            if (ret == 0)
            {
                LOG_INFO("zero file");
                // do_send(socketfd, -U, 0, 0, NULL);
                return;
            }
            else if (ret == -1)
            {
                ERROR_EXIT("read");
            }
            LOG_INFO("tmp exists");

            //validate MD5
            uint8_t hash[16];
            memset(hash, 0, sizeof(hash));

            md5(buf, offset, hash);
            // send the 0 package
            // ------------------------
            // | CMD | 1 | size | MD5 |
            // ------------------------
            do_send(socketfd, -U, 1, offset, hash);

            free(buf);
        }
        else
        {
            do_send(socketfd, -U, 0, 0, NULL);
        }
        printf("receiving upload file: %s, size: %.2f M\n", fileName, fileSize / 1024.0 / 1024.0);
        return;
    }

    if (NULL == fileName)
    {
        // didnt sent 0 package, communication fail
        LOG_FATAL("peer didn't send 0 package");
    }

    if (fileSize < 0)
    {
        // communication error
        LOG_ERROR("file size error");
        fileSize = 0; // reenterable
        return;
    }

    // write to file
    if (fd > 0)
    {
        // reuse fd
    }
    else
    {
        fd = open(fileName, O_RDWR | O_CREAT, 0666);
        if (fd < 0)
        {
            ERROR_EXIT("open");
        }
    }

    // seq = 1
    if (1 == metaData->param)
    {
        resetCurser(fd);
        offset = 0;
    }

    if (offset != 0)
    {
        fileSize -= offset;
        offset = 0;
    }

    printf("recv %.2f M\r", 1.0 * metaData->param / packageSize);

    int dataSize = fileSize < packageSize ? fileSize : packageSize;

    // remain data size
    fileSize -= packageSize;

    int ret = write(fd, metaData->data, dataSize);
    if (-1 == ret)
    {
        ERROR_EXIT("write to file");
    }
    else if (0 == ret)
    {
        LOG_INFO("write finished?");
    }
    else if (dataSize == ret)
    {
        // wirte ok
    }
    else
    {
        LOG_WARN("write incomplete");
    }

    LOG_DEBUG("%s remain size is %ld", plainName, fileSize);

    // transmit complete
    if (fileSize <= 0)
    {
        LOG_INFO("receive complete");

        //rename file
        if (-1 == rename(fileName, plainName))
        {
            LOG_FATAL("rename fail");
        }
        else
        {
            LOG_INFO("rename to %s", plainName);
            printf("rename to %s", plainName);
        }

        // clean work
        free(fileName);
        fileName = NULL;
        free(plainName);
        plainName = NULL;

        close(fd);
        fd = 0;

        fileSize = 0;

        printf("\n");

        // client get server file
        if (isClient)
        {
            showOpt();
        }
    }
}

int isTmpExists(int socketfd, const char *fileName)
{
    LOG_TRACE("isTmpExists");
    if (-1 == access(fileName, R_OK | W_OK | F_OK))
    {
        LOG_WARN("no tmp file found");
        return 0;
    }
    return 1;
}

char *prepareDataBuf(const char *buf, int size)
{
    LOG_TRACE("prepareDataBuf %d", size);
    char *data = malloc(size);
    memcpy(data, buf, size);
    return data;
}

void releaseDataBuf(char *buf)
{
    LOG_TRACE("releaseDataBuf");
    free(buf);
}

void transmitFileFrame(int socketfd, int filefd, char *buf, uint64_t seq, uint64_t sum)
{
    const int packageSize = 1024;
    LOG_INFO("have %ld packages to send", sum - seq + 1);
    // a slow loop, so we should put interactive method here in
    while (seq <= sum)
    {
        // get back thread received option
        interactive(socketfd, getOptNotify(), resetOptNotify);

        if (isTransferSuspend())
        {
            printf("\rsuspending......\r");
            // busy wait
            sleep(0.5);
            continue;
        }

        //TODO: big file would show no progress
        float progress = 1.0 * seq / sum * 100;
        printf("progress %.2f %%", progress);

        memset(buf, 0, packageSize); // never sizeof (char *)
        int ret = read(filefd, buf, packageSize);
        if (ret == 0)
        {
            LOG_INFO("end of file");
            // return
        }
        else if (ret == -1)
        {
            ERROR_EXIT("read");
        }
        else
        {
            // send the seq package
            // ------------------------
            // | CMD | 0 | seq | data |
            // ------------------------
            do_send(socketfd, U, 0, seq, buf);
        }
        seq++; // seq num auto incr

        printf("\r");
    }
    printf("\nupload finished\n");
    //close(filefd);
}
