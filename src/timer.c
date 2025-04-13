#include "../include/timer.h"

char* timer_timestamp(){
    time_t raw_time;
    struct tm *time_info;
    char buffer[100];

    time(&raw_time);                      // Get current time
    time_info = localtime(&raw_time);     // Convert to local time

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info);

    return buffer;
}