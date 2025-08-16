#include "../include/timer.h"

char* timer_timestamp(){
    time_t now = time(NULL);
    return ctime(&now);
}