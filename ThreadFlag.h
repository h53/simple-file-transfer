#ifndef THREADFLAG_H__
#define THREADFLAG_H__

#include "common.h"
#include "Logger.h"
#include "dispatcher.h"

extern pthread_mutex_t g_mutex;
extern volatile int g_optFlag;    // thread communicate

// static int notify(int opt);

void *getOptInBackground(void *arg);

int getOptNotify();

void resetOptNotify();

#endif