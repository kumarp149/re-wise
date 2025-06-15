#ifndef __LOG_H__
#define __LOG_H__

#include "commit.h"
#include "print.h"
#include "args.h"

#define __LOG_HELP__ __CONSTANTS_RW_PGMNAME__ " log <archive> <arg> <val>"

#define __LOG_FLAGBIT_HELP_ 1

#define __LOG_FLAGS__ \
X("--help","-h" , "help", "display this usage", (__LOG_FLAGBIT_HELP_)) \

void process_log(int argc,char** argv);


#endif
