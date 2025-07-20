#ifndef __STATUS__
#define __STATUS__

#include "constants.h"
#include "stdbool.h"
#include "args.h"
#include "commit.h"
#include "print.h"
#include "errors.h"
#include "regex.h"

#define __STATUS_HELP__ __CONSTANTS_RW_PGMNAME__ " status <archive> <arg> <val>"

#define __STATUS_FLAGBIT_HELP__ 1

#define __STATUS_FLAGS__ \
  X("--help","-h" , "help","display this usage", (__STATUS_FLAGBIT_HELP__)) \

#define __STATUS_ARGS__ \
  X(false,"--path","-p","path","display the status of specific paths (regex supported)",(1<<20)) \

#define __STATUS_MODIFIED__ "modified:"

#define __STATUS_DELETED__ "deleted:"

#define __STATUS_CREATED__ "created:"

void process_status(int argc,char** argv);

enum path_status{
  MODIFIED,
  CREATED,
  DELETED,
  UNCHANGED,
  IRRELEVANT
};

bool status_is_worktree_clean(struct zip* archive,struct hash_map* worktree);

#endif