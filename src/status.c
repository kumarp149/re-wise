#include "../include/status.h"

void show_status_usage(struct args_flag* flags,size_t flags_size,struct args_valarg* valargs,size_t valargs_size){
    printf("usage: " __STATUS_HELP__ "\n\n");
    printf("Following are the available flags and arguments:\n");

    for (int i=0;i<flags_size;++i){
        printf("  %s|%s: %s\n",(flags+i)->shortId,(flags+i)->longId,(flags+i)->long_description);
    }
    for (int i=0;i<valargs_size;++i){
        printf("  %s|%s: %s\n",(valargs+i)->shortId,(valargs+i)->longId,(valargs+i)->long_description);
    }
}



void process_status(int argc,char** argv){

    struct args_flag flags[] = {
        #define X(longId, shortId, short_description, long_description, flagId) { longId, shortId, short_description, long_description, flagId },
            __STATUS_FLAGS__
        #undef X
    };

    struct args_valarg valargs[] = {
        #define X(mandatory, longId, shortId, short_description, long_description, maxCount) {mandatory, longId, shortId, short_description, long_description, maxCount },
            __STATUS_ARGS__
        #undef X
    };

    char** options_array[__ARGS_OPTION_TYPES__];
    int* options_sizes[__ARGS_OPTION_TYPES__];

    struct zip* archive;

    int command_flags = 0;

    int args_error_status = 0;

    char* error_message = (char *)malloc(sizeof(char)*1000);

    int zip_open_error = 0;

    processArgs(argc,argv, &flags, sizeof(flags)/sizeof(flags[0]), &valargs, sizeof(valargs)/sizeof(valargs[0]), &archive, &zip_open_error, &command_flags, options_array, options_sizes, &args_error_status, &error_message);

    if (((command_flags) && (1<<2)) == 1){
        show_status_usage(&flags, sizeof(flags)/sizeof(flags[0]), &valargs, sizeof(valargs)/sizeof(valargs[0]));
        return;
    } else if (args_error_status != 0){
        printf("%s\n",error_message);
        show_status_usage(&flags, sizeof(flags)/sizeof(flags[0]), &valargs, sizeof(valargs)/sizeof(valargs[0]));
        return;
    } else if (zip_open_error == ZIP_ER_NOENT){
        printf("error: the archive %s not found\n",argv[2]);
        return;
    } else if (zip_open_error == ZIP_ER_NOZIP){
        printf("error: %s is not a valid archive\n",argv[2]);
        return;
    } else if (zip_open_error == ZIP_ER_OPEN){
        printf("error: cannot open the file %s\n",argv[2]);
        return;
    } else if (zip_open_error > 0){
        printf("error: unknown error occurred\n");
        return;
    }

    char* head_commit = commit_get_head_commit(archive);

    //printf("head_commit: %s\n",head_commit);

    struct jvc_commit* head_commit_obj = commit_get_commit(archive,head_commit);

    struct hash_map* hash_map_current_state = iterate_zip(archive);

    /*free all the below three*/
    char** only_current = (char **)malloc(sizeof(char *)*1000);
    char** only_index = (char **)malloc(sizeof(char *)*1000);
    char** both = (char **) malloc(sizeof(char *)*1000);

    size_t only_current_size = 0;
    size_t only_index_size = 0;
    size_t both_size = 0;

    bool does_changes_exist = false;

    map_get_difference(head_commit_obj->tree,hash_map_current_state,only_index,&only_index_size,only_current,&only_current_size,both,&both_size);

    if (only_index_size > 0){
        does_changes_exist = true;
        printf("following files have been deleted\n");

        for (int i=0;i<only_index_size;++i){
            printf(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_RED__ "   %s\n" __CONSTANTS_RW_COLOR_END__,only_index[i]);
        }
    }

    if (only_current_size > 0){
        does_changes_exist = true;
        printf(__STATUS_CREATED__ "\n");
        //printf(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_GREEN__ __STATUS_CREATED__ __CONSTANTS_RW_COLOR_END__ "\n");

        for (int i=0;i<only_current_size;++i){
            printf(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_GREEN__ "   %s\n" __CONSTANTS_RW_COLOR_END__,only_current[i]);
        }
    }

    if (both_size > 0){
        does_changes_exist = true;
        printf("following files have been modified\n");

        for (int i=0;i<both_size;++i){
            if (strcmp(hash_map_get(head_commit_obj->tree,both[i]),hash_map_get(hash_map_current_state,both[i])) != 0){
                printf(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_YELLOW__ "   %s\n" __CONSTANTS_RW_COLOR_END__,both[i]);
            }
        }
    }
    if (head_commit){
        free(head_commit);
        head_commit = NULL;
    }

    if (head_commit_obj){
        commit_free(&head_commit_obj);
    }
    if (does_changes_exist) printf("commit the changes to track them\n");
}