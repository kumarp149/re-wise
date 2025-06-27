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

void log_commits(struct zip* archive,int max_commits_to_log){
    struct jvc_commit* commit = commit_get_commit(archive,commit_get_head_commit(archive));

    int logged_commits = 0;

    while(commit != NULL){
        show_message(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_YELLOW__ "commit: %s\n" __CONSTANTS_RW_COLOR_END__,commit->id);
        show_message("  %s",commit->message);

        logged_commits++;

        char* parent_commit_id = commit_get_parent_commit_id(archive,commit->id);

        if (parent_commit_id == NULL) break;

        commit = commit_get_commit(archive,parent_commit_id);

        if (commit != NULL){
            show_message("");
        }

        if (logged_commits >= max_commits_to_log) break;
    }

    return;
}

void process_log(int argc,char** argv) {
    struct args_flag flags[] = {
        #define X(longId, shortId, short_description, long_description, flagId) { longId, shortId, short_description, long_description, flagId },
        __LOG_FLAGS__
        #undef X
    };

    struct args_valarg valargs[] = {
        #define X(mandatory, longId, shortId, short_description, long_description, maxCount) {mandatory, longId, shortId, short_description, long_description, maxCount },
            __LOGS__ARGS__
        #undef X
    };

    struct zip* archive;

    int command_flags = 0;

    char** options_array[__ARGS_OPTION_TYPES__];
    int options_sizes[__ARGS_OPTION_TYPES__] = {0};

    char integer_args[] = __LOGS__INTEGERARGS__; 

    int proceed_further;

    processArgs(argc,argv,flags,sizeof(flags)/sizeof(flags[0]),valargs,sizeof(valargs)/sizeof(valargs[0]),&archive,&command_flags,options_array, options_sizes,show_log_usage,&proceed_further,__COMMIT_FLAGBIT_HELP_,integer_args,1);

    if (proceed_further != 1) return;

    int max_commits_to_log = 100;

    if (options_sizes['c'-'a'] > 0){
        max_commits_to_log = atoi(options_array['c'-'a'][0]);
    }    

    log_commits(archive,max_commits_to_log);
}