#ifndef __COMMIT__
#define __COMMIT__

#include "tree.h"
#include "utils.h"
#include "blob.h"
#include "args.h"
#include "sha256.h"
#include "timer.h"
#include "global.h"
#include "errors.h"
#include "constants.h"

#define __COMMIT_TREE_PREFIX__ "tree: "
#define __COMMIT_PARENT_PREFIX__ "parent: "
#define __COMMIT_MESSAGE_PREFIX__ "message: "
#define __COMMIT_HEADER__ "COMMIT"

#define __COMMIT_HELP__ __CONSTANTS_RW_PGMNAME__ " commit <archive> <arg> <val>"

#define __COMMIT_FLAGBIT_HELP_ 1
#define __COMMIT_FLAGBIT_EMPTY_ 2

#define __COMMIT_FLAGS__ \
  X("--help","-h" , "help", "display this usage", (__COMMIT_FLAGBIT_HELP_)) \
  X("--empty","-e", "empty", "to create an empty commit", (__COMMIT_FLAGBIT_EMPTY_)) \

#define __COMMIT_ARGS__ \
  X(false,"--path","-p","path","list of paths to include for the commit",(1<<20)) \
  X(true,"--message","-m","message","commit message to commit the change",1) \

struct jvc_commit{
    char* id;
    struct jvc_commit* parent;
    struct jvc_tree* tree;
    char* message;
    //need to include timestamp too
};

void commit_add_blob_ng(struct zip* file,struct jvc_commit* commit);

void commit_add_blob(struct zip* file,struct jvc_commit* commit);

void commit_append_tree(char* blob_content,struct jvc_commit* commit,size_t* sz);

char* commit_get_head_commit(struct zip* archive);

struct jvc_commit* commit_get_commit(struct zip* archive,char *id);

void commit_free_commit(struct jvc_commit** commit);

void process_commit(int argc,char** argv);

bool commit_is_valid(struct zip* archive,char* commit_id);

char* commit_resolve_commit(struct zip* archive,char *identifier,char** message);

char* commit_get_parent_commit_id(struct zip* archive,char *commit_id);

#endif