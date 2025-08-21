#ifndef __BLOB__
#define __BLOB__

#include <stdlib.h>
#include "constants.h"
#include "string.h"
#include "commit.h"
#include "zip.h"
#include "stdutils.h"

/*free the returned pointer after usage*/
char* blob_get_path(const char* blob_id);

char* blob_get_hash_atrevision(struct zip* archive,char* commit_id,char* path);

char* blob_get_type_path(const char* blob_id);

char* blob_get_commitid(char *commit_object_path);

#endif
