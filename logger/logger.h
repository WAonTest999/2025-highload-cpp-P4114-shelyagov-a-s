#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#ifdef LOG_E
#define LOG_ERROR(msg) LOG("\033[31m[ERROR]\033[39m", msg)
#ifndef LOG_W
#define LOG_W
#endif
#else
#define LOG_ERROR(msg)
#endif


#ifdef LOG_W
#define LOG_WARN(msg) LOG("\033[33m[WARN]\033[39m", msg)
#ifndef LOG_I
#define LOG_I
#endif
#else
#define LOG_WARN(msg)
#endif

#ifdef LOG_I
#define LOG_INFO(msg) LOG("\033[34m[INFO]\033[39m", msg)
#else
#define LOG_INFO(msg)
#endif



#ifdef LOG_ON
#define LOG(lvl, msg) ({            \
    fprintf(stderr, "%s\t%s:%d :: %s\n", \
    lvl, __FILE__, __LINE__, msg);})
#else
#define LOG(lvl, msg) 
#endif


#endif