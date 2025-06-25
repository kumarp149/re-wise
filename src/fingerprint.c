#include "../include/fingerprint.h"

void show_fingerprint_usage(struct args_flag* flags,size_t flags_size,struct args_valarg* valargs,size_t valargs_size){
    show_message("usage: " __FINGERPRINT_HELP__ "\n");
    show_message("Following are the available flags and arguments:");

    for (size_t i=0;i<flags_size;++i){
        show_message("  %s|%s: %s",(flags+i)->shortId,(flags+i)->longId,(flags+i)->long_description);
    }
    for (size_t i=0;i<valargs_size;++i){
        show_message("  %s|%s: %s",(valargs+i)->shortId,(valargs+i)->longId,(valargs+i)->long_description);
    }
}

void fingerprint(struct zip* archive,char* commit_identifier,char* path){
    struct jvc_commit* commit;
    if (commit_identifier == NULL){
        zip_file_t* file = zip_fopen(archive,path,0);

        if (file){
            show_message(sha256_zip_file_ng(file));
            zip_fclose(file);
        } else{
            show_message("the path %s is not found in the zip",path);
        }
    } else{
        char* commit_resolve_message;

        char* commit_id = commit_resolve_commit(archive,commit_identifier,&commit_resolve_message);

        if (commit_id == NULL){
            commit_id = tag_resolve_tag(archive,commit_identifier,&commit_resolve_message);

            if (commit_id == NULL){
                show_message(commit_resolve_message);
                return;
            }
        }

        if (!commit_is_valid(archive,commit_id)){
            show_message("%s is not a valid commit",commit_id);
            return;
        }
        commit = commit_get_commit(archive,commit_get_head_commit(archive));

        if (hash_map_get(commit->tree->map,path) != NULL){
            show_message(hash_map_get(commit->tree->map,path));
        } else{
            show_message("the path %s not found at the commit %s",path,commit_id);
        }
    }
}


void process_fingerprint(int argc,char** argv) {
    struct args_flag flags[] = {
        #define X(longId, shortId, short_description, long_description, flagId) { longId, shortId, short_description, long_description, flagId },
        __FINGERPRINT_FLAGS__
        #undef X
    };

    struct args_valarg valargs[] = {
        #define X(mandatory, longId, shortId, short_description, long_description, maxCount) {mandatory, longId, shortId, short_description, long_description, maxCount },
            __FINGERPRINT_ARGS__
        #undef X
    };

    struct zip* archive;

    int command_flags = 0;

    char** options_array[__ARGS_OPTION_TYPES__];
    int options_sizes[__ARGS_OPTION_TYPES__] = {0};

    int proceed_further;

    processArgs(argc,argv,flags,sizeof(flags)/sizeof(flags[0]),valargs,sizeof(valargs)/sizeof(valargs[0]),&archive,&command_flags,options_array, options_sizes,show_fingerprint_usage,&proceed_further,__FINGERPRINT_FLAGBIT_HELP_);

    if (proceed_further != 1) return;

    char* commit_identifier = NULL;

    if (options_sizes['c'-'a'] == 1) commit_identifier = options_array['c'-'a'][0];

    fingerprint(archive,commit_identifier,options_array['p'-'a'][0]);
}