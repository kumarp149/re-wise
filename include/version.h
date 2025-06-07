#pragma once

#include <stdlib.h>
#include "constants.h"
#include "args.h"
#include "utils.h"
#include "print.h"
#include "stdbool.h"
#include "commit.h"

#define __VERSION_HELP__ __CONSTANTS_RW_PGMNAME__ " version <archive> <arg> <val>"

#define __VERSION_FLAGBIT_HELP__ 1
#define __VERSION_FLAGBIT_FORCE__ 2

#define __VERSION_FLAGS__ \
  X("--help","-h" , "help", "display this usage", (__VERSION_FLAGBIT_HELP__)) \
  X("--force","-f","force","force alter the tag",(__VERSION_FLAGBIT_FORCE__))

#define __VERSION_ARGS__ \
  X(true,"--commit","-c","commit","the commit to version",1) \
  X(true,"--name","-n","name","name of the version",1) 

void process_version(int argc,char** argv);