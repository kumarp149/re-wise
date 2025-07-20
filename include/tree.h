#ifndef __TREE__
#define __TREE__

#include "structures.h"
#include "constants.h"
#include "blob.h"
#include "utils.h"

struct jvc_tree{
    char* id;
    struct hash_map* map;
};

void tree_add_blob(struct zip* archive, struct jvc_tree* tree);

struct jvc_tree* tree_get_tree(struct zip* archive,char* id);

void tree_free(struct jvc_tree** tree);

struct hash_map* tree_getworktree(struct zip* archive);


#endif