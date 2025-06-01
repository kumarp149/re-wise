#include "../include/timer.h"

char* timer_timestamp(){
    time_t raw_time;
    struct tm *time_info;
    char* buffer = (char *)malloc(sizeof(char)*__CONSTANT_RW_STRING_BUFFER_S);

    time(&raw_time);                      // Get current time
    time_info = localtime(&raw_time);     // Convert to local time

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info);

    return buffer;
}