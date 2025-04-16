#include "../include/print.h"

void show_message(const char* format, ...){
    va_list args;
    va_start(args,format);
    vprintf(format,args);
    va_end(args);
    printf("\n");
}

void log_message(const char* format, ...){
#if __PRINT_LOG__
    va_list args;
    va_start(args,format);
    vprintf(format,args);
    va_end(args);
    printf("\n");
#endif
}