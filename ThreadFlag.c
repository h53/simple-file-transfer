#include "ThreadFlag.h"

pthread_mutex_t g_mutex;
volatile int g_optFlag = 0; // thread communicate

void *getOptInBackground(void *arg)
{
    for (;;)
    {
        if (g_optFlag == 0)
        {
            pthread_mutex_lock(&g_mutex);
            if (g_optFlag == 0)
            {
                g_optFlag = getOpt();
            }
            pthread_mutex_unlock(&g_mutex);
        }
        sleep(1);
    }
    return NULL;
}

int getOptNotify()
{
    int opt = 0;
    if (g_optFlag != 0)
    {
        pthread_mutex_lock(&g_mutex);
        if (g_optFlag != 0)
        {
            opt = g_optFlag;
        }
        pthread_mutex_unlock(&g_mutex);
        LOG_TRACE("get notify %d", opt);
    }
    return opt;
}

void resetOptNotify()
{
    if (g_optFlag != 0)
    {
        pthread_mutex_lock(&g_mutex);
        if (g_optFlag != 0)
        {
            g_optFlag = 0;
        }
        pthread_mutex_unlock(&g_mutex);
        LOG_TRACE("reset notify");
    }
}