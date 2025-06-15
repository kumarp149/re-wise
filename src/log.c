#include "../include/log.h"

void show_log_usage(struct args_flag* flags,size_t flags_size,struct args_valarg* valargs,size_t valargs_size){
    show_message("usage: " __LOG_HELP__ "\n");
    show_message("Following are the available flags and arguments:");

    for (size_t i=0;i<flags_size;++i){
        show_message("  %s|%s: %s",(flags+i)->shortId,(flags+i)->longId,(flags+i)->long_description);
    }
    for (size_t i=0;i<valargs_size;++i){
        show_message("  %s|%s: %s",(valargs+i)->shortId,(valargs+i)->longId,(valargs+i)->long_description);
    }
}

void log_commits(struct zip* archive){
    struct jvc_commit* commit = commit_get_commit(archive,commit_get_head_commit(archive));

    while(commit != NULL){
        show_message("%s, %s",commit->id,commit->message);

        char* parent_commit_id = commit_get_parent_commit_id(archive,commit->id);

        if (parent_commit_id == NULL) break;

        commit = commit_get_commit(archive,parent_commit_id);
    }

    return;
}

void process_log(int argc,char** argv) {
    struct args_flag flags[] = {
        #define X(longId, shortId, short_description, long_description, flagId) { longId, shortId, short_description, long_description, flagId },
        __LOG_FLAGS__
        #undef X
    };

    struct args_valarg valargs[] = {};

    struct zip* archive;

    int command_flags = 0;

    char** options_array[__ARGS_OPTION_TYPES__];
    int options_sizes[__ARGS_OPTION_TYPES__] = {0};

    int proceed_further;

    processArgs(argc,argv,flags,sizeof(flags)/sizeof(flags[0]),valargs,sizeof(valargs)/sizeof(valargs[0]),&archive,&command_flags,options_array, options_sizes,show_log_usage,&proceed_further,__COMMIT_FLAGBIT_HELP_);

    if (proceed_further != 1) return;

    log_commits(archive);
}