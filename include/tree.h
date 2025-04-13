#ifndef __TREE__
#define __TREE__

#include "structures.h"
#include "constants.h"

struct jvc_tree{
    char* id;
    struct hash_map* map;
};

void add_tree_object(struct jvc_tree* tree);



#endif