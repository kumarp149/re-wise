#ifndef __ARGS__
#define __ARGS__

#include <stdbool.h>
#include <zip.h>
#include <stdlib.h>
#include "string.h"

/*
convention for arguments:
1) all the args be it valarg or flag should come after the name of the archive
2) basically, the archive should be third argument (index 2)
*/
#define __ARGS_OPTION_TYPES__ 26

struct args_flag{
    char* longId;
    char* shortId;
    char* short_description;
    char* long_description;
    int flagId;
};

struct args_valarg{
    bool mandatory;
    char* longId;
    char* shortId;
    char* short_description;
    char* long_description;
    int maxCount;
};

void processArgs(int argc,char** argv,struct args_flag* flags,size_t flags_size,struct args_valarg* valargs,size_t valargs_size,zip_t** archive,int* archive_open_error,int* flag,char ***option_values, int *option_counts,int* err,char** error_message);

#endif