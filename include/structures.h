#ifndef __STRUCTURES__
#define __STRUCTURES__

#define JVC_HASHMAP_SIZE 1024
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "global.h"
#include "error.h"
#include "stdutils.h"

struct jvc_index{
    struct hash_map* map;
};

struct jvc_head{
    char* id;
};

struct jvc_blob{
    char* id;
    char* data;
};

#endif