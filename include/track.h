#ifndef __TRACK__
#define __TRACK__

#include "structures.h"
#include "utils.h"
#include <time.h>
#include "sha256.h"
#include "timer.h"
#include "commit.h"
#include "constants.h"
#include <stdio.h>
#include "args.h"
#include "tree.h"
#include "print.h"
#include "string.h"

/*

*/

/*track takes only options (which are basically indicators) and the archive name*/

#define __TRACK_HELP__ __CONSTANTS_RW_PGMNAME__ " track <archive> <flags> <arg> <value>"

#define __TRACK_FLAGBIT_FORCE__ 1
#define __TRACK_FLAGBIT_HELP__ 2

#define __TRACK_FLAGS__ \
  X("--force", "-f", "force", "flag to force re-init the archive", (__TRACK_FLAGBIT_FORCE__)) \
  X("--help","-h" , "help","show the usage", (__TRACK_FLAGBIT_HELP__)) \

#define __TRACK_ARGS__ \
  X(true,"--message","-m","message","commit message to commit the archive",1) \

#define __TRACK_OPTIONS__ \
    X("--force|-f", "force track the zip from scratch") \
    X("--help|-h", "show the usage") \


extern const char* track_option_names[];
extern const char* track_option_descriptions[];
extern const char* track_argument_names[];
extern const char* track_argument_description[];

#define __TRACK_FLAG_FORCE__ 1<<0


hash_map* load_options();
    

void process_track(int argc,char** argv);


#endif