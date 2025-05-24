#include "../include/status.h"

void show_status_usage(struct args_flag* flags,size_t flags_size,struct args_valarg* valargs,size_t valargs_size){
    printf("usage: " __STATUS_HELP__ "\n\n");
    printf("Following are the available flags and arguments:\n");

    for (size_t i=0;i<flags_size;++i){
        printf("  %s|%s: %s\n",(flags+i)->shortId,(flags+i)->longId,(flags+i)->long_description);
    }
    for (size_t i=0;i<valargs_size;++i){
        printf("  %s|%s: %s\n",(valargs+i)->shortId,(valargs+i)->longId,(valargs+i)->long_description);
    }
}

void show_all_diff(hash_map* index,hash_map* current,hash_map* only_index,hash_map* only_current,hash_map* both){
    log_message("showing all diff");
    bool no_changes = true;
    if (hash_map_isempty(only_index) == false){
        no_changes = false;
        log_message("only index is not empty");
        show_message(__STATUS_DELETED__);
        for (int i=0;i<JVC_HASHMAP_SIZE;++i){
            hash_node* node = only_index->buckets[i];
            while(node){
                show_message(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_RED__ "   %s" __CONSTANTS_RW_COLOR_END__,node->key);
                node = node->next;
            }
        }
    }

    if (hash_map_isempty(only_current) == false){
        no_changes = false;
        show_message("\n" __STATUS_CREATED__);
        for (int i=0;i<JVC_HASHMAP_SIZE;++i){
            hash_node* node = only_current->buckets[i];
            while(node){
                show_message(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_GREEN__ "   %s" __CONSTANTS_RW_COLOR_END__,node->key);
                node = node->next;
            }
        }
    }

    bool isDiffFound = false;

    if (hash_map_isempty(both) == false){
        for (int i=0;i<JVC_HASHMAP_SIZE;++i){
            hash_node* node = both->buckets[i];
            while(node){
                if (strcmp(hash_map_get(index,node->key),hash_map_get(current,node->key)) != 0){
                    if (isDiffFound == false){
                        show_message("\n" __STATUS_MODIFIED__);
                        isDiffFound = true;
                        no_changes = false;
                    }
                    show_message(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_YELLOW__ "   %s" __CONSTANTS_RW_COLOR_END__,node->key);
                }
                node = node->next;
            }
        }
    }

    log_message("no_changes: %d\n",no_changes);

    if (no_changes == true){
        show_message("the archive is clean, no changes to commit");
    }
}

void show_particular_diff(hash_map* index,hash_map* current,hash_map* only_index,hash_map* only_current,hash_map* both,char** options_array,size_t options_size){
    log_message("inside show_particular_diff");
    log_message("size of options: %d",options_size);
    for (size_t i=0;i<options_size;++i){
        char* opt = options_array[i];
        log_message("going for opt: %s",opt);
        if (hash_map_get(only_current,opt) != NULL){
            show_message(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_GREEN__ "%s: " __CONSTANTS_RW_COLOR_END__ "created",opt);
        } else if (hash_map_get(only_index,opt) != NULL){
            show_message(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_RED__ "%s: " __CONSTANTS_RW_COLOR_END__ "deleted",opt);
        } else if (hash_map_get(both,opt) != NULL){
            log_message("hashes: %s, %s",hash_map_get(index,opt),hash_map_get(current,opt));
            if (strcmp(hash_map_get(index,opt),hash_map_get(current,opt)) != 0){
                show_message(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_YELLOW__ "%s: " __CONSTANTS_RW_COLOR_END__ "modified",opt);
            } else{
                show_message(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_YELLOW__ "%s: " __CONSTANTS_RW_COLOR_END__ "unchanged",opt);
            }
        } else{
            show_message("%s: " "not found in the zip",opt);
        }
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
    int options_sizes[__ARGS_OPTION_TYPES__] = {0};

    struct zip* archive;

    int command_flags = 0;

    int args_error_status = 0;

    char* error_message = (char *)malloc(sizeof(char)*1000);

    int zip_open_error = 0;

    processArgs(argc,argv, flags, sizeof(flags)/sizeof(flags[0]), valargs, sizeof(valargs)/sizeof(valargs[0]), &archive, &zip_open_error, &command_flags, options_array, options_sizes, &args_error_status, &error_message);

    if (((command_flags) & (1<<2)) == 1){
        show_status_usage(flags, sizeof(flags)/sizeof(flags[0]), valargs, sizeof(valargs)/sizeof(valargs[0]));
        return;
    } else if (args_error_status != 0){
        printf("%s\n",error_message);
        show_status_usage(flags, sizeof(flags)/sizeof(flags[0]), valargs, sizeof(valargs)/sizeof(valargs[0]));
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

    log_message("head commit is %s",head_commit);

    //printf("head_commit: %s\n",head_commit);

    struct jvc_commit* head_commit_obj = commit_get_commit(archive,head_commit);

    struct hash_map* hash_map_current_state = iterate_zip(archive);

    /*free all the below three*/
    hash_map* map_only_first = create_hash_map();
    hash_map* map_only_second = create_hash_map();
    hash_map* map_both = create_hash_map();

    bool does_changes_exist = false;


    log_message("options size: %d\n",*(options_sizes + ('p'-'a')));

    // for (int i=0;i<options_sizes['p'-'a'];++i){
    //     log_message("%s",options_array['p'-'a'][i]);
    // }
    map_get_difference(head_commit_obj->tree->map,hash_map_current_state,map_only_first,map_only_second,map_both);

    if (options_sizes['p'-'a'] == 0){
        log_message("calling show_all_diff");
        show_all_diff(head_commit_obj->tree->map,hash_map_current_state,map_only_first,map_only_second,map_both);
    } else{
        log_message("calling show_particular_diff");
        show_particular_diff(head_commit_obj->tree->map,hash_map_current_state,map_only_first,map_only_second,map_both,options_array['p'-'a'],(size_t) options_sizes['p'-'a']);
    }
    
    if (does_changes_exist) printf("commit the changes to track them\n");
}