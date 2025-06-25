#ifndef __FINGERPRINT__
#define __FINGERPRINT__

#include <stdlib.h>
#include "constants.h"
#include "args.h"
#include "commit.h"
#include "tag.h"

#define __FINGERPRINT_HELP__ __CONSTANTS_RW_PGMNAME__ " fingerprint <archive> <arg> <val>"

#define __FINGERPRINT_FLAGBIT_HELP_ 1

#define __FINGERPRINT_FLAGS__ \
  X("--help","-h" , "help", "display this usage", (__FINGERPRINT_FLAGBIT_HELP_)) \

#define __FINGERPRINT_ARGS__ \
  X(false,"--commit","-c","commit","commit",1) \
  X(true,"--path","-p","path","path",1) \

void process_fingerprint(int argc,char** argv);

#endif