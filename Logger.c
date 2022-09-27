#include "Logger.h"

static LogLevel logLevel = INFO;

void setLogLevel(LogLevel level)
{
    logLevel = level;
}

LogLevel getLogLevel(void)
{
    return logLevel;
}

// int main()
// {
//     setLogLevel(TRACE);
//     LOG_TRACE("%s", "hello");
//     LOG_DEBUG("%d", 15);
//     LOG_INFO("%c", 'a');
//     LOG_WARN("%x", 15);
//     LOG_ERROR("%o",8);
//     LOG_FATAL("%s", "FATAL");
    
//     LOG_FATAL("hello");

//     LOG_TRACE("init server %s %d", "ip", 12);
// }