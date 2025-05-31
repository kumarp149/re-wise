#ifndef __RESTORE__
#define __RESTORE__

#include <stdbool.h>
#include "args.h"
#include "constants.h"
#include "blob.h"
#include "utils.h"
#include "print.h"


#define __RESTORE_HELP__ __CONSTANTS_RW_PGMNAME__ " restore <archive> <arg> <val>"

#define __RESTORE_FLAGBIT_HELP__ 1

#define __RESTORE_FLAGS__ \
  X("--help","-h" , "help","display this usage", (__RESTORE_FLAGBIT_HELP__)) \

#define __RESTORE_ARGS__ \
  X(true,"--path","-p","path","restore the specific paths from the HEAD commit (regex unsupported)",(1<<20)) \

void process_restore(int argc,char** argv);

#endif