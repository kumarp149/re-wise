#include "../include/restore.h"

void show_restore_usage(struct args_flag* flags,size_t flags_size,struct args_valarg* valargs,size_t valargs_size){
    printf("usage: " __RESTORE_HELP__ "\n\n");
    printf("Following are the available flags and arguments:\n");

    for (size_t i=0;i<flags_size;++i){
        printf("  %s|%s: %s\n",(flags+i)->shortId,(flags+i)->longId,(flags+i)->long_description);
    }
    for (size_t i=0;i<valargs_size;++i){
        printf("  %s|%s: %s\n",(valargs+i)->shortId,(valargs+i)->longId,(valargs+i)->long_description);
    }
}

void process_restore(int argc,char** argv){
    struct args_flag flags[] = {
        #define X(longId, shortId, short_description, long_description, flagId) { longId, shortId, short_description, long_description, flagId },
            __RESTORE_FLAGS__
        #undef X
    };

    struct args_valarg valargs[] = {
        #define X(mandatory, longId, shortId, short_description, long_description, maxCount) {mandatory, longId, shortId, short_description, long_description, maxCount },
            __RESTORE_ARGS__
        #undef X
    };

    if (strcmp(argv[2],"-h") == 0 || strcmp(argv[2],"--help") == 0){
        show_restore_usage(flags, sizeof(flags)/sizeof(flags[0]), valargs, sizeof(valargs)/sizeof(valargs[0]));
        return;
    }

    char** options_array[__ARGS_OPTION_TYPES__];
    int options_sizes[__ARGS_OPTION_TYPES__] = {0};

    struct zip* archive;

    int command_flags = 0;

    int args_error_status = 0;

    char* error_message = (char *)malloc(sizeof(char)*1000);

    ErrorCode errorCode = ERR_NOERROR;

    processArgs(argc,argv, flags, sizeof(flags)/sizeof(flags[0]), valargs, sizeof(valargs)/sizeof(valargs[0]), &archive, &command_flags, options_array, options_sizes, &args_error_status, &error_message, &errorCode);

    if (((command_flags) & (1<<__RESTORE_FLAGBIT_HELP__)) != 0){
        show_restore_usage(flags, sizeof(flags)/sizeof(flags[0]), valargs, sizeof(valargs)/sizeof(valargs[0]));
        return;
    } else if (args_error_status != 0){
        show_message("%s\n",error_message);
        show_restore_usage(flags, sizeof(flags)/sizeof(flags[0]), valargs, sizeof(valargs)/sizeof(valargs[0]));
        return;
    } else if (errorCode != ERR_NOERROR){
        show_message(error_get_message(errorCode));
        return;
    }
    return;
    
}