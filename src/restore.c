#include "../include/restore.h"

void show_restore_usage(struct args_flag* flags,size_t flags_size,struct args_valarg* valargs,size_t valargs_size){
    show_message("usage: " __RESTORE_HELP__ "\n");
    show_message("Following are the available flags and arguments:");

    for (size_t i=0;i<flags_size;++i){
        show_message("  %s|%s: %s",(flags+i)->shortId,(flags+i)->longId,(flags+i)->long_description);
    }
    for (size_t i=0;i<valargs_size;++i){
        show_message("  %s|%s: %s",(valargs+i)->shortId,(valargs+i)->longId,(valargs+i)->long_description);
    }
}

void restore_path(struct zip* archive,char* path){
    char* head_commit = commit_get_head_commit(archive);

    char* path_hash_in_head = blob_get_hash_atrevision(archive,head_commit,path);

    if (path_hash_in_head == NULL){
        show_message("the path %s is not found at HEAD",path);
        return;
    }

    zip_file_t* file = zip_fopen(archive,path,0);
    
    char* path_hash_in_worktree = sha256_zip_file_ng(file);

    if (path_hash_in_worktree == NULL){
        goto __BLOBCOPY;
    }

    if (strcmp(path_hash_in_head,path_hash_in_worktree) == 0){
        show_message("path %s is unchanged",path);
        return;
    }

__BLOBCOPY:

    copy_file_inzip_ng(archive,blob_get_path(path_hash_in_head),path);
    return;
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

    for (int i=0;i<options_sizes['p'-'a'];++i){
        restore_path(archive,options_array['p'-'a'][i]);
    }

    zip_close(archive);
    return;
}