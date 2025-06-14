#pragma once

#include <stdlib.h>
#include "constants.h"
#include "args.h"
#include "utils.h"
#include "print.h"
#include "stdbool.h"
#include "commit.h"

#define __TAG_HELP__ __CONSTANTS_RW_PGMNAME__ " tag <archive> <arg> <val>"

#define __TAG_FLAGBIT_HELP__ 1
#define __TAG_FLAGBIT_FORCE__ 2

#define __TAG_FLAGS__ \
  X("--help","-h" , "help", "display this usage", (__TAG_FLAGBIT_HELP__)) \
  X("--force","-f","force","force alter the tag",(__TAG_FLAGBIT_FORCE__))

#define __TAG_ARGS__ \
  X(true,"--commit","-c","commit","the commit to tag",1) \
  X(true,"--name","-n","name","name of the tag",1) 

void process_tag(int argc,char** argv);