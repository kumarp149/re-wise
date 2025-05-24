#ifndef __COMMIT__
#define __COMMIT__

#include "tree.h"
#include "utils.h"
#include "blob.h"
#include "args.h"
#include "sha256.h"
#include "timer.h"

#define __COMMIT_TREE_PREFIX__ "tree: "
#define __COMMIT_PARENT_PREFIX__ "parent: "
#define __COMMIT_MESSAGE_PREFIX__ "message: "

#define __COMMIT_HELP__ __CONSTANTS_RW_PGMNAME__ " commit <archive> <arg> <val>"

#define __COMMIT_FLAGS__ \
  X("--help","-h" , "help","display this usage", (1<<2)) \

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

void commit_add_blob(struct zip* file,struct jvc_commit* commit);

void commit_append_tree(char* blob_content,struct jvc_commit* commit,size_t* sz);

char* commit_get_head_commit(struct zip* archive);

struct jvc_commit* commit_get_commit(struct zip* archive,char *id);

// void commit_free(struct jvc_commit** commit);

void process_commit(int argc,char** argv);

#endif