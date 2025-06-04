#ifndef __ARGS__
#define __ARGS__

#include <stdbool.h>
#include <zip.h>
#include <stdlib.h>
#include "string.h"
#include "print.h"
#include "errors.h"

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

/*
free the following variables later
flags
valargs
option_values
error_message
*/

#define __ARGS_SHOW_USAGE_ show_usage(flags, flags_size, valargs, valargs_size)
void processArgs(int argc,char** argv, struct args_flag* flags, size_t flags_size, struct args_valarg* valargs, size_t valargs_size, zip_t** archive, int* flag,char ***option_values, int *option_counts,void (*show_usage)(struct args_flag* flags,size_t flags_size,struct args_valarg* valargs,size_t valargs_size),int* proceed_further,int help_bit);
#endif