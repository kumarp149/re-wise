/**
 * @file args.h
 * @brief Parsing command-line arguments
*/

#ifndef __ARGS__
#define __ARGS__

#include <stdbool.h>
#include <zip.h>
#include <stdlib.h>
#include "string.h"
#include "print.h"
#include "errors.h"
#include "global.h"
#include "utils.h"
#include "stdutils.h"
#include "structs/strvec.h"

/*
convention for arguments:
1) all the args be it valarg or flag should come after the name of the archive
2) basically, the archive should be third argument (index 2)
*/
#define __ARGS_OPTION_TYPES__ 26
#define __ARGS_ERROR_ARG_MORE_PROVIDED__ "error: more than necessary arguments provided for the option <%s>"
#define __ARGS_ERROR_ARG_MANDATORY_NOTPROVIDED__ "error: argument <%s> is mandatory and it expects a value"
#define __ARGS_ERROR_ARG_ATLEAST_ONE__ "error: option <%s> expects atleast one argument"
#define __ARGS_ERROR_ARG_NONMANDATORY_NOTPROVIDED__ "error: option <%s> expects an argument"
#define __ARGS_ERROR_ARG_UNKNOWNARG__ "error: unknown argument <%s> provided"
#define __ARGS_ERROR_ARG_MANDATORY_NOTGIVEN__ "error: the argument <%s> is mandatory"
#define __ARGS_ERROR_ARG_NUMERIC_ARG__ "error: the argument <%s> expects a positive integer value"

#define __ARGS_SHOW_USAGE_ show_usage(flags, flags_size, valargs, valargs_size)

/**
 * @struct args_flag
 * @brief struct to define a flag type argument
 */
struct args_flag{
    char* longId;
    char* shortId;
    char* short_description;
    char* long_description;
    int flagId;
};

/**
 * @struct args_valarg
 * @brief struct to define an argument that takes value(s)
 */
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

/**
 * @brief process the arguments
 *
 * Preconditions:
 * - Path must not already contain a repository
 *
 * Postconditions:
 * - Repository structure is created on disk
 *
 * @param argc number of arguments given
 * @param argv all the arguments given
 * @param flags list of all flag type arguments supported by that particular command
 * @param flags_size size of the flags variable
 * @param valargs list of all option type arguments supported by that particular command
 * @param archive pointer to the archive which needs to be set by the caller
 * @return 0 on success, negative on failure
 */
void processArgs(int argc,char** argv, struct args_flag* flags, size_t flags_size, struct args_valarg* valargs, size_t valargs_size, zip_t** archive, int* flag,char ***option_values, int *option_counts,void (*show_usage)(struct args_flag* flags,size_t flags_size,struct args_valarg* valargs,size_t valargs_size),int* proceed_further,int help_bit,char* integer_args,size_t integer_args_size);
#endif