#ifndef __BLOB__
#define __BLOB__

#include <stdlib.h>
#include "constants.h"
#include "string.h"

/*free the returned pointer after usage*/
char* blob_get_path(const char* blob_id);

#endif
