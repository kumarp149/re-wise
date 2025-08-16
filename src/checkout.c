#include "../include/checkout.h"

void show_checkout_usage(struct args_flag* flags,size_t flags_size,struct args_valarg* valargs,size_t valargs_size){
    show_message("usage: " __CHECKOUT_HELP__ "\n");
    show_message("Following are the available flags and arguments:");

    for (size_t i=0;i<flags_size;++i){
        show_message("  %s|%s: %s",(flags+i)->shortId,(flags+i)->longId,(flags+i)->long_description);
    }
    for (size_t i=0;i<valargs_size;++i){
        show_message("  %s|%s: %s",(valargs+i)->shortId,(valargs+i)->longId,(valargs+i)->long_description);
    }
}

void process_checkout(int argc,char** argv){
    struct args_flag flags[] = {
        #define X(longId, shortId, short_description, long_description, flagId) { longId, shortId, short_description, long_description, flagId },
            __CHECKOUT_FLAGS__
        #undef X
    };

    struct args_valarg valargs[] = {
        #define X(mandatory, longId, shortId, short_description, long_description, maxCount) {mandatory, longId, shortId, short_description, long_description, maxCount },
            __CHECKOUT_ARGS__
        #undef X
    };

    struct zip* archive;

    int command_flags = 0;

    char** options_array[__ARGS_OPTION_TYPES__];
    int options_sizes[__ARGS_OPTION_TYPES__] = {0};

    int proceed_further;

    processArgs(argc,argv,flags,sizeof(flags)/sizeof(flags[0]),valargs,sizeof(valargs)/sizeof(valargs[0]),&archive,&command_flags,options_array,options_sizes,show_checkout_usage,&proceed_further,__CHECKOUT_FLAGBIT_HELP__,NULL,0);

    if (proceed_further != 1) return;

    struct hash_map* worktree = tree_getworktree(archive);

    if (status_is_worktree_clean(archive,worktree) == false){
        show_message("the archive has some uncommitted changes. please commit them to make sure you don't lose them");
        return;
    }

    char* commit_resolve_message;

    struct jvc_commit* commit = commit_get_commit(archive,commit_resolve_commit(archive,options_array['c'-'a'][0],&commit_resolve_message));

    if (commit == NULL){
        show_message("no commit found with the given id");
        return;
    }

    struct jvc_commit* head_commit = commit_get_commit(archive,commit_resolve_commit(archive,"HEAD",&commit_resolve_message));

    if (strcmp(commit->id,head_commit->id) == 0){
        show_message("already at the commit. no need to checkout");
        return;
    }

    for (int i=0;i<JVC_HASHMAP_SIZE;++i){
        hash_node* node = worktree->buckets[i];
        while(node){
            char* file_hash_inworktree = node->value;
            char* file_path = node->key;
            char* file_hash_at_targetcommit = hash_map_get(commit->tree->map,(const char*) file_path);
            
            if (file_hash_at_targetcommit == NULL){
                delete_files_inzip_ng(archive,(const char **)&file_path,1);
            } else if (file_hash_at_targetcommit != file_hash_inworktree){
                copy_file_inzip_ng(archive,blob_get_path(file_hash_at_targetcommit),file_path);
            }
            node = node->next;
        }
    }

    for (int i=0;i<JVC_HASHMAP_SIZE;++i){
        hash_node* node = commit->tree->map->buckets[i];
        while(node){
            if (hash_map_get(worktree,node->key) == NULL){
                copy_file_inzip_ng(archive,blob_get_path(node->value),node->key);
            }
            node = node->next;
        }
    }

    write_to_file_inzip_ng(archive,__CONSTANTS_RW_BASE__ __CONSTANTS_RW_HEAD__,commit->id,strlen(commit->id));
    
    zip_close(archive);

    // commit_free_commit(&commit);

    // commit_free_commit(&head_commit);

    // free_hash_map(worktree);

    return;
}