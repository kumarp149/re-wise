#ifndef __DIFF__
#define __DIFF__

#include "constants.h"
#include <stdlib.h>
#include "args.h"
#include "commit.h"


#define __DIFF_HELP__ __CONSTANTS_RW_PGMNAME__ " diff <archive> <arg> <val>"

#define __DIFF_FLAGBIT_HELP_ 1

#define __DIFF_FLAGS__ \
  X("--help","-h" , "help", "display this usage", (__DIFF_FLAGBIT_HELP_)) \

  #define __DIFF_ARGS__ \
  X(false,"--path","-p","path","compares the given pathspecs in both commits",(1<<20)) \
  X(true,"--commits","-c","two commits for diff","two commits for the difference",2) \


void process_diff(int argc,char** argv);

#endif