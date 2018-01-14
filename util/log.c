#include <stdio.h>
#include <stdarg.h>

#include <log.h>



void ldebug(const char *__restrict __fmt, ...)
{
   va_list ap;  
   va_start(ap,__fmt);  
   char buffer[4096];
   vsnprintf(buffer, 4096, __fmt, ap);

   printf("debug:%s\n", buffer);

   va_end(ap); 
}

void linfo(const char *__restrict __fmt, ...)
{
   va_list ap;  
   va_start(ap,__fmt);  
   char buffer[4096];
   vsnprintf(buffer, 4096, __fmt, ap);

   printf("info:%s\n", buffer);

   va_end(ap); 
}

void lwarn(const char *__restrict __fmt, ...)
{
   va_list ap;  
   va_start(ap,__fmt);  
   char buffer[4096];
   vsnprintf(buffer, 4096, __fmt, ap);

   printf("warn:%s\n", buffer);

   va_end(ap); 
}

void lerror(const char *__restrict __fmt, ...)
{
   va_list ap;  
   va_start(ap,__fmt);  
   char buffer[4096];
   vsnprintf(buffer, 4096, __fmt, ap);

   printf("error:%s\n", buffer);

   va_end(ap); 
}

btch_log log = {
    ldebug, linfo, lwarn, lerror
};


