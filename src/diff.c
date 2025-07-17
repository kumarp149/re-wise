#include "../include/diff.h"

void show_diff_usage(struct args_flag* flags,size_t flags_size,struct args_valarg* valargs,size_t valargs_size){
    show_message("usage: " __DIFF_HELP__ "\n");
    show_message("Following are the available flags and arguments:");

    for (size_t i=0;i<flags_size;++i){
        show_message("  %s|%s: %s",(flags+i)->shortId,(flags+i)->longId,(flags+i)->long_description);
    }
    for (size_t i=0;i<valargs_size;++i){
        show_message("  %s|%s: %s",(valargs+i)->shortId,(valargs+i)->longId,(valargs+i)->long_description);
    }
}

void process_diff(int argc,char** argv){
    struct args_flag flags[] = {
        #define X(longId, shortId, short_description, long_description, flagId) { longId, shortId, short_description, long_description, flagId },
        __DIFF_FLAGS__
        #undef X
    };

    struct args_valarg valargs[] = {
        #define X(mandatory, longId, shortId, short_description, long_description, maxCount) {mandatory, longId, shortId, short_description, long_description, maxCount },
            __DIFF_ARGS__
        #undef X
    };

    struct zip* archive;

    int command_flags = 0;

    char** options_array[__ARGS_OPTION_TYPES__];
    int options_sizes[__ARGS_OPTION_TYPES__] = {0};

    int proceed_further;

    processArgs(argc,argv,flags,sizeof(flags)/sizeof(flags[0]),valargs,sizeof(valargs)/sizeof(valargs[0]),&archive,&command_flags,options_array, options_sizes,show_diff_usage,&proceed_further,__DIFF_FLAGBIT_HELP_,NULL,0);

    if (proceed_further != 1) return;

    char* commit_resolve_message;

    struct jvc_commit* commit1 = commit_get_commit(archive,commit_resolve_commit(archive,options_array['c'-'a'][0],&commit_resolve_message));

    struct jvc_commit* commit2 = commit_get_commit(archive,commit_resolve_commit(archive,options_array['c'-'a'][1],&commit_resolve_message));

    if (commit1 == NULL){
        show_message("no commit found with the given id: %s",options_array['c'-'a'][0]);
        return;
    }

    if (commit2 == NULL){
        show_message("no commit found with the given id: %s",options_array['c'-'a'][1]);
        return;
    }

    struct jvc_tree* tree1 = commit1->tree;
    struct jvc_tree* tree2 = commit2->tree;

    for (int i=0;i<JVC_HASHMAP_SIZE;++i){
        hash_node* node = tree1->map->buckets[i];
        while(node){
            if (hash_map_get(tree2->map,node->key) == NULL){
                show_message(node->key);
            } else if (strcmp(node->value,hash_map_get(tree2->map,node->key)) != 0){
                show_message(node->key);
            }
            node = node->next;
        }
    }

    for (int i=0;i<JVC_HASHMAP_SIZE;++i){
        hash_node* node = tree2->map->buckets[i];
        while(node){
            if (hash_map_get(tree1->map,node->key) == NULL){
                show_message(node->key);
            }
            node = node->next;
        }
    }

    commit_free_commit(&commit1);
    commit_free_commit(&commit2);
    

    zip_close(archive);
}
