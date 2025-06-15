#include "../include/tag.h"

char* tag_get_path(const char* commit_id){
    size_t version_path_size = strlen(__CONSTANTS_RW_BASE__) + strlen(__CONSTANTS_RW_TAGS__) + strlen(commit_id) + 1;

    size_t sz = 0;

    char* version_path = (char *)malloc(sizeof(char) * version_path_size);

    memcpy(version_path + sz,__CONSTANTS_RW_BASE__,strlen(__CONSTANTS_RW_BASE__));

    sz += strlen(__CONSTANTS_RW_BASE__);

    memcpy(version_path + sz,__CONSTANTS_RW_TAGS__,strlen(__CONSTANTS_RW_TAGS__));

    sz += strlen(__CONSTANTS_RW_TAGS__);

    memcpy(version_path + sz,commit_id,strlen(commit_id));

    sz += strlen(commit_id);

    version_path[sz] = '\0';

    return version_path;
}

void show_tag_usage(struct args_flag* flags,size_t flags_size,struct args_valarg* valargs,size_t valargs_size){
    show_message("usage: " __TAG_HELP__ "\n");
    show_message("Following are the available flags and arguments:");

    for (size_t i=0;i<flags_size;++i){
        show_message("  %s|%s: %s",(flags+i)->shortId,(flags+i)->longId,(flags+i)->long_description);
    }
    for (size_t i=0;i<valargs_size;++i){
        show_message("  %s|%s: %s",(valargs+i)->shortId,(valargs+i)->longId,(valargs+i)->long_description);
    }
}

void process_tag(int argc,char** argv){

    struct args_flag flags[] = {
        #define X(longId, shortId, short_description, long_description, flagId) { longId, shortId, short_description, long_description, flagId },
            __TAG_FLAGS__
        #undef X
    };

    struct args_valarg valargs[] = {
        #define X(mandatory, longId, shortId, short_description, long_description, maxCount) {mandatory, longId, shortId, short_description, long_description, maxCount },
            __TAG_ARGS__
        #undef X
    };

    char** options_array[__ARGS_OPTION_TYPES__];
    int options_sizes[__ARGS_OPTION_TYPES__] = {0};

    struct zip* archive;

    int command_flags = 0;

    int proceed_further;

    processArgs(argc,argv,flags,sizeof(flags)/sizeof(flags[0]),valargs,sizeof(valargs)/sizeof(valargs[0]),&archive,&command_flags,options_array, options_sizes,show_tag_usage,&proceed_further,__TAG_FLAGBIT_HELP__);

    if (proceed_further != 1) return;

    char* commit_resolve_message;

    char* commit_id = commit_resolve_commit(archive,options_array['c'-'a'][0],&commit_resolve_message);

    if (commit_id == NULL){
        show_message(commit_resolve_message);
        return;
    }
    
    char* tag_name = options_array['n'-'a'][0];

    char* tag_path = tag_get_path(tag_name);

    if (!commit_is_valid(archive,commit_id)){
        show_message("%s is not a valid commit",commit_id);
        return;
    }

    if (file_exists_inzip_ng(archive,tag_path) == true){
        if (!(((command_flags) & (1 << __TAG_FLAGBIT_FORCE__)) != 0)){
            show_message("tag %s already exists",tag_name);
            return;
        }
    }

    write_to_file_inzip_ng(archive,tag_path,commit_id,strlen(commit_id));

    show_message("the commit %s is tagged as %s",commit_id,tag_name);

    zip_close(archive);

    //__RW_MEMFREE__(commit_id);
}