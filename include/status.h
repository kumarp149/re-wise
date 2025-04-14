#ifndef __STATUS__
#define __STATUS__

#include "constants.h"
#include "stdbool.h"
#include "args.h"
#include "commit.h"

#define __STATUS_HELP__ JVC_PGM_NAME " status <archive> <arg> <val>"

#define __STATUS_FLAGS__ \
  X("--help","-h" , "help","display this usage", (1<<2)) \

#define __STATUS_ARGS__ \
  X(false,"--path","-p","path","display the status of specific paths",(1<<20)) \

#define __STATUS_MODIFIED__ "following files have been modified"

#define __STATUS_DELETE__ "following files have been deleted"

#define __STATUS_CREATED__ "following files have been created"

void process_status(int argc,char** argv);


#endif