#ifndef __BTCH_LOG__
#define __BTCH_LOG__


typedef struct btch_log{
    void (*debug)(const char *__restrict __fmt, ...);
    void (*info)(const char *__restrict __fmt, ...);
    void (*warn)(const char *__restrict __fmt, ...);
    void (*error)(const char *__restrict __fmt, ...);
}btch_log;


extern btch_log  log;



#endif