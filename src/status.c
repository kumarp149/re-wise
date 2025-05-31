#include "../include/status.h"

void show_status_usage(struct args_flag* flags,size_t flags_size,struct args_valarg* valargs,size_t valargs_size){
    show_message("usage: " __STATUS_HELP__ "\n");
    show_message("Following are the available flags and arguments:");

    for (size_t i=0;i<flags_size;++i){
        show_message("  %s|%s: %s",(flags+i)->shortId,(flags+i)->longId,(flags+i)->long_description);
    }
    for (size_t i=0;i<valargs_size;++i){
        show_message("  %s|%s: %s",(valargs+i)->shortId,(valargs+i)->longId,(valargs+i)->long_description);
    }
}

void show_all_diff(hash_map* index,hash_map* current,hash_map* only_index,hash_map* only_current,hash_map* both,char** options_array,size_t options_size){
    log_message("showing all diff");
    bool no_changes = true;
    bool no_changes_in_matching_paths = true;
    if (hash_map_isempty(only_index) == false){
        no_changes = false;
        log_message("only index is not empty");
        bool flag = false;
        for (int i=0;i<JVC_HASHMAP_SIZE;++i){
            hash_node* node = only_index->buckets[i];
            while(node){
                if (options_size == 0 || regex_is_anypattern_matching(options_array,options_size,node->key)){
                    if (flag == false){
                        flag = true;
                        show_message(__STATUS_DELETED__);
                    }
                    no_changes_in_matching_paths = false;
                    show_message(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_RED__ "   %s" __CONSTANTS_RW_COLOR_END__,node->key);
                }
                node = node->next;
            }
        }
    }

    if (hash_map_isempty(only_current) == false){
        no_changes = false;
        bool flag = false;
        for (int i=0;i<JVC_HASHMAP_SIZE;++i){
            hash_node* node = only_current->buckets[i];
            while(node){
                if (options_size == 0 || regex_is_anypattern_matching(options_array,options_size,node->key)){
                    if (flag == false){
                        flag = true;
                        show_message("\n" __STATUS_CREATED__);
                    }
                    no_changes_in_matching_paths = false;
                    show_message(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_GREEN__ "   %s" __CONSTANTS_RW_COLOR_END__,node->key);
                }
                node = node->next;
            }
        }
    }

    if (hash_map_isempty(both) == false){
        for (int i=0;i<JVC_HASHMAP_SIZE;++i){
            hash_node* node = both->buckets[i];
            bool flag = false;
            while(node){
                if (strcmp(hash_map_get(index,node->key),hash_map_get(current,node->key)) != 0){
                    if (options_size == 0 || regex_is_anypattern_matching(options_array,options_size,node->key)){
                        if (flag == false){
                            flag = true;
                            show_message("\n" __STATUS_MODIFIED__);
                        }
                        no_changes_in_matching_paths = false;
                        show_message(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_YELLOW__ "   %s" __CONSTANTS_RW_COLOR_END__,node->key);
                    }
                }
                node = node->next;
            }
        }
    }

    log_message("no_changes: %d\n",no_changes);

    if (no_changes == true){
        show_message("the archive is clean, no changes to commit");
    } else if (no_changes_in_matching_paths == true){
        show_message("no changes in the pathspecs given");
    }
}

// void show_particular_diff(hash_map* index,hash_map* current,hash_map* only_index,hash_map* only_current,hash_map* both,char** options_array,size_t options_size){
//     log_message("inside show_particular_diff");
//     log_message("size of options: %d",options_size);
//     for (size_t i=0;i<options_size;++i){
//         char* opt = options_array[i];
//         log_message("going for opt: %s",opt);
//         if (hash_map_get(only_current,opt) != NULL){
//             show_message(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_GREEN__ "%s: " __CONSTANTS_RW_COLOR_END__ "created",opt);
//         } else if (hash_map_get(only_index,opt) != NULL){
//             show_message(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_RED__ "%s: " __CONSTANTS_RW_COLOR_END__ "deleted",opt);
//         } else if (hash_map_get(both,opt) != NULL){
//             log_message("hashes: %s, %s",hash_map_get(index,opt),hash_map_get(current,opt));
//             if (strcmp(hash_map_get(index,opt),hash_map_get(current,opt)) != 0){
//                 show_message(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_YELLOW__ "%s: " __CONSTANTS_RW_COLOR_END__ "modified",opt);
//             } else{
//                 show_message(__CONSTANTS_RW_COLOR_START__ __CONSTANTS_RW_COLOR_YELLOW__ "%s: " __CONSTANTS_RW_COLOR_END__ "unchanged",opt);
//             }
//         } else{
//             show_message("%s: " "not found in the zip",opt);
//         }
//     }
// }


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

    if (strcmp(argv[2],"-h") == 0 || strcmp(argv[2],"--help") == 0){
        show_status_usage(flags, sizeof(flags)/sizeof(flags[0]), valargs, sizeof(valargs)/sizeof(valargs[0]));
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

    if (((command_flags) & (1<<__STATUS_FLAGBIT_HELP__)) != 0){
        show_status_usage(flags, sizeof(flags)/sizeof(flags[0]), valargs, sizeof(valargs)/sizeof(valargs[0]));
        return;
    } else if (args_error_status != 0){
        show_message("%s\n",error_message);
        show_status_usage(flags, sizeof(flags)/sizeof(flags[0]), valargs, sizeof(valargs)/sizeof(valargs[0]));
        return;
    } else if (errorCode != ERR_NOERROR){
        show_message(error_get_message(errorCode));
        return;
    }

    char* head_commit = commit_get_head_commit(archive);

    log_message("head commit is %s",head_commit);

    struct jvc_commit* head_commit_obj = commit_get_commit(archive,head_commit);

    struct hash_map* hash_map_current_state = iterate_zip(archive);

    /*free all the below three*/
    hash_map* map_only_first = create_hash_map();
    hash_map* map_only_second = create_hash_map();
    hash_map* map_both = create_hash_map();

    bool does_changes_exist = false;


    log_message("options size: %d\n",*(options_sizes + ('p'-'a')));

    map_get_difference(head_commit_obj->tree->map,hash_map_current_state,map_only_first,map_only_second,map_both);

    show_all_diff(head_commit_obj->tree->map,hash_map_current_state,map_only_first,map_only_second,map_both,options_array['p'-'a'],(size_t) options_sizes['p'-'a']);
    
    if (does_changes_exist) show_message("commit the changes to track them\n");
}

// enum path_status status_get_path_status(struct zip* archive,char* path){
//     char* head_commit = commit_get_head_commit(archive);

//     bool path_ispresent_inhead = false;
//     bool path_ispresent_intree = false;

//     char* path_hash_in_head = status_get_path_hash(path,head_commit,&path_ispresent_inhead);



//     zip_file_t* file = zip_fopen(archive,path,0);

//     char* path_hash_work_tree = sha256_zip_file_ng(file);

//     if (path_hash_work_tree){
//         path_ispresent_intree = true;
//     }

//     if (path_ispresent_inhead == false && path_ispresent_intree == false){
//         return IRRELEVANT;
//     } else if (path_ispresent_inhead == false){
//         return CREATED;
//     } else if (path_ispresent_intree == false){
//         return DELETED;
//     } else{
//         if (strcmp(path_hash_in_head,path_hash_work_tree) == 0){
//             return UNCHANGED;
//         } else return MODIFIED;
//     }
// }

// char* status_get_path_hash(char* path,char* commit_id,bool* path_ispresent_inhead){
    
// }