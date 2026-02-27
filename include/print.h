#ifndef __PRINT__
#define __PRINT__

#include "stdio.h"
#include <stdarg.h>

#define __PRINT_LOG__ 1

void show_message(const char* format, ...);

void log_message(const char* fotmat, ...);

#endif