#ifndef LOGGER_H__
#define LOGGER_H__

#include "common.h"
typedef enum
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
}LogLevel;

void setLogLevel(LogLevel level);

LogLevel getLogLevel(void);

#define LOG_TRACE(format, ...) if(getLogLevel() <= TRACE) \
    fprintf(stderr, "%s %d :" format "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_DEBUG(format, ...) if(getLogLevel() <= DEBUG) \
    fprintf(stderr, "%s %d :" format "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_INFO(format, ...) if(getLogLevel() <= INFO) \
    fprintf(stderr, "%s %d :" format "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_DEBUG(format, ...) if(getLogLevel() <= DEBUG) \
    fprintf(stderr, "%s %d :" format "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_WARN(format, ...) if(getLogLevel() <= WARN) \
    fprintf(stderr, "%s %d :" format "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_ERROR(format, ...) if(getLogLevel() <= ERROR) \
    fprintf(stderr, "%s %d :" format "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_FATAL(format, ...) do { \
    fprintf(stderr, "%s %d :" format "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    exit(EXIT_FAILURE); } while(0)

#endif