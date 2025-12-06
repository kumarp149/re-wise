#ifndef __BLOB__
#define __BLOB__

#include <stdlib.h>
#include "constants.h"
#include "string.h"
#include "zip.h"
#include "stdutils.h"
#include "commit.h"

struct blob{
    int type; /*type can be 1 (for OBJECT), 2 for (COMMIT), 3 for (TREE)*/
    char* id;
    char* content;
    char* src;
};

struct copy_ctx{
    struct blob* blob;
    zip_file_t *zf;
    int eof;
};


#define __BLOB_OBJECTTYPE__ 1
#define __BLOB_COMMITTYPE__ 2
#define __BLOB_TREETYPE__   3
#define __BLOB_CHUNK_SIZE__ 2048


/*free the returned pointer after usage*/
char* blob_get_path(const char* blob_id);

char* blob_get_hash_atrevision(struct zip* archive,char* commit_id,char* path);

char* blob_get_type_path(const char* blob_id);

char* blob_get_commitid(char *commit_object_path);

void blob_write_blob(struct blob* blob,struct zip* archive);

int blob_get_type(struct zip* archive,char *id);

#endif
