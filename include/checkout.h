#ifndef __CHECKOUT__
#define __CHECKOUT__

#include <stdlib.h>
#include "args.h"
#include "commit.h"
#include "tree.h"
#include "status.h"
#include "utils.h"
#include "blob.h"
#include "tag.h"

#define __CHECKOUT_HELP__ __CONSTANTS_RW_PGMNAME__ " commit <archive> <arg> <val>"

#define __CHECKOUT_FLAGBIT_HELP__ 1

#define __CHECKOUT_FLAGS__ \
  X("--help","-h" , "help", "display this usage", (__CHECKOUT_FLAGBIT_HELP__)) \


#define __CHECKOUT_ARGS__ \
  X(true,"--commit","-c","commit","commitid to checkout to",1) \

void process_checkout(int argc,char** argv);

#endif