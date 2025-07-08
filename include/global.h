#ifndef __GLOBAL__
#define __GLOBAL__

char* __JVC_ERROR_MESSAGE__;

#define __RW_MEMFREE__(p) do { if ((p) != NULL) { free(p); (p) = NULL; } } while (0)

#define __RW_ARGSERROR_SIZE 100

#endif