#include "../include/track.h"

const char* track_option_names[] = {
    #define X(opt, desc) opt,
    __TRACK_OPTIONS__
    #undef X
};

const char* track_option_descriptions[] = {
    #define X(opt, desc) desc,
    __TRACK_OPTIONS__
    #undef X
};

bool is_already_initialized(struct zip* archive){
    if (file_exists_inzip_ng(archive,__CONSTANTS_RW_BASE__ __CONSTANTS_RW_HEAD__)) return true;
    return false;
}

/*cleans the archive and forms hash_generators for both index and tree*/
void clean_archive(zip_t* archive, hash_map* map,struct sha256_generator* tree_generator,struct sha256_generator* commit_generator){
    show_message("started cleaning archive");

    zip_uint64_t num_entries = (zip_uint64_t) zip_get_num_entries(archive, 0);

    for (zip_uint64_t i = 0; i < num_entries; i++){
        const char *name = zip_get_name(archive, i,  ZIP_FL_UNCHANGED);
        size_t len = strlen(name);

        if (len > 0 && name[len - 1] == '/'){
            continue;
        }

        if (name && strncmp(name, __CONSTANTS_RW_BASE__, strlen(__CONSTANTS_RW_BASE__)) == 0){
            zip_delete(archive, i);
        } else{

            zip_file_t* file = zip_fopen(archive,name,0);
            char* file_hash = sha256_zip_file_ng(file);
            hash_map_insert(map,name,file_hash);
            log_message("inserting %s to hash_map with %s",name,file_hash);
            zip_fclose(file);

            char* time = timer_timestamp();

            sha256_update_content(tree_generator,__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__,strlen(__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__));
            log_message("going to next one");
            sha256_update_content(tree_generator,name,strlen(name));
            log_message("going to next two");
            sha256_update_content(tree_generator,__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__,strlen(__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__));
            sha256_update_content(tree_generator,time,strlen(time));

            //free(time);

            time = timer_timestamp();

            sha256_update_content(commit_generator,__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__,strlen(__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__));
            sha256_update_content(commit_generator,name,strlen(name));
            sha256_update_content(commit_generator,__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__,strlen(__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__));
            sha256_update_content(commit_generator,name,strlen(name));
            sha256_update_content(commit_generator,__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__,strlen(__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__));
            sha256_update_content(commit_generator,time,strlen(time));

            //free(time);
            free(file_hash);
        }
    }
    show_message("cleaned archive");
}


void track(struct zip* archive, int flags,char ***option_values,struct jvc_tree** tree_blob,struct jvc_commit** commit_blob){
    bool is_archive_being_tracked = is_already_initialized(archive);
    if ((is_archive_being_tracked) && !(flags & (1<<__TRACK_FLAGBIT_FORCE__))){
        show_message("the archive is already being tracked");
        return;
    }
    show_message("inside track");

    hash_map* map_path_hash = create_hash_map();

    struct sha256_generator* tree_generator = sha256_create_new_generator();

    struct sha256_generator* commit_generator = sha256_create_new_generator();

    show_message("before clean_archive");

    clean_archive(archive,map_path_hash,tree_generator,commit_generator);

    //log_message("size of hash_map: %d",(int) hash_map_getsize(map_path_hash));

    for (int i=0;i<JVC_HASHMAP_SIZE;++i){
        hash_node* node = map_path_hash->buckets[i];
        while(node){
            log_message("node pointer: %p, key pointer: %p", (void*)node, (void*)node->key);
            if (!node->key){
                node = node->next;
                continue;
            }
            log_message("before copying: %s",node->key);
            copy_file_from_worktree(archive,node->key);
            node = node->next;
        }
    }

    *tree_blob = (struct jvc_tree *) malloc(sizeof(struct jvc_tree));

    (*tree_blob)->id = sha256_string(tree_generator->data,tree_generator->sz);
    (*tree_blob)->map = map_path_hash;

    *commit_blob = (struct jvc_commit *)malloc(sizeof(struct jvc_commit));

    (*commit_blob)->id = strdup(sha256_string(commit_generator->data,commit_generator->sz));
    (*commit_blob)->parent = NULL;
    (*commit_blob)->tree = *tree_blob;
    (*commit_blob)->message = option_values['m'-'a'][0];

    commit_add_blob(archive,*commit_blob);
    log_message("added the commit blob: %s",(*commit_blob)->id);
    tree_add_blob(archive,*tree_blob);
    log_message("added the tree blob: %s",(*tree_blob)->id);
    write_to_file_inzip_ng(archive,__CONSTANTS_RW_BASE__ __CONSTANTS_RW_HEAD__,(*commit_blob)->id,strlen((*commit_blob)->id));

    log_message("done with the track");
}

/*checks if head and index are already present*/
void show_track_usage(){
    show_message("usage: "__TRACK_HELP__"\n");
    show_message("Following are the available flags and arguments:");
    
    for (unsigned long long i=0;i<sizeof(track_option_names)/sizeof(track_option_names[0]);++i){
        show_message("  %-10s: %s", track_option_names[i], track_option_descriptions[i]);
    }
}

/*load all the options*/
hash_map* load_options(){
    hash_map* options_map = create_hash_map();
    for (unsigned long long i=0;i < sizeof(track_option_names)/sizeof(track_option_names[0]);++i){
        char* token = strtok(strdup(track_option_names[i]),"|");
        while(token != NULL){
            hash_map_insert(options_map,token,track_option_descriptions[i]);
            token = strtok(NULL,"|");
        }
        // hash_map_insert(options_map,track_option_names[i],track_option_descriptions[i]);
    }
    return options_map;
}

void process_track(int argc,char** argv){
    show_message("starting track command");
    
    if (argc == 2){
        show_track_usage();
    } if (argc == 3){
        if (strcmp(argv[2],"--h") == 0 || strcmp(argv[2],"--help") == 0){
            show_track_usage();
            return;
        }
    }

    struct args_flag flags[] = {
        #define X(longId, shortId, short_description, long_description, flagId) { longId, shortId, short_description, long_description, flagId },
            __TRACK_FLAGS__
        #undef X
    };

    struct args_valarg valargs[] = {
        #define X(mandatory, longId, shortId, short_description, long_description, maxCount) {mandatory, longId, shortId, short_description, long_description, maxCount },
            __TRACK_ARGS__
        #undef X
    };

    char** options_array[__ARGS_OPTION_TYPES__];
    int options_sizes[__ARGS_OPTION_TYPES__] = {0};

    struct zip* archive;

    int command_flags = 0;

    int proceed_further;

    processArgs(argc,argv,flags,sizeof(flags)/sizeof(flags[0]),valargs,sizeof(valargs)/sizeof(valargs[0]),&archive,&command_flags,options_array, options_sizes,show_track_usage,&proceed_further,__TRACK_FLAGBIT_HELP__,NULL,0);

    show_message("processArgs done");

    if (proceed_further != 1) return;

    struct jvc_tree* tree;
    struct jvc_commit* commit;

    show_message("before track");

    track(archive,command_flags,options_array,&tree,&commit);

    log_message("done with the track operation");

    log_message("process_track done");

    if (zip_close(archive) < 0) {
    fprintf(stderr, "zip_close failed: %s\n",
            zip_strerror(archive));
}

    // if (commit != NULL && commit->id != NULL){
    //     show_message("started tracking the archive. %s is the root commit",commit->id);
    //     commit_free_commit(&commit);
    // }

    return;
}