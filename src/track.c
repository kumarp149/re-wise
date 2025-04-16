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
    if (file_exists_inzip_ng(archive,__CONSTANTS_RW_BASE__ __CONSTANTS_RW_HEAD__) && file_exists_inzip_ng(archive,__CONSTANTS_RW_BASE__ __CONSTANTS_RW_INDEX__)) return true;
    return false;
}

/*cleans the archive and forms hash_generators for both index and tree*/
void clean_archive(zip_t* archive, hash_map* map,struct sha256_generator* tree_generator,struct sha256_generator* commit_generator){

    zip_int64_t num_entries = zip_get_num_entries(archive, 0);

    for (zip_int64_t i = num_entries - 1; i >= 0; i--){
        const char *name = zip_get_name(archive, i, 0);

        if (name && strncmp(name, __CONSTANTS_RW_BASE__, strlen(__CONSTANTS_RW_BASE__)) == 0){
            zip_delete(archive, i);
        } else{

            zip_file_t* file = zip_fopen(archive,name,0);
            char* file_hash = sha256_zip_file_ng(file);
            hash_map_insert(map,name,file_hash);
            //printf("inserting key: %s\n",name);
            zip_fclose(file);

            char* time = timer_timestamp();

            sha256_update_content(tree_generator,__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__,strlen(__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__));
            sha256_update_content(tree_generator,name,strlen(file_hash));
            sha256_update_content(tree_generator,__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__,strlen(__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__));
            sha256_update_content(tree_generator,time,strlen(time));

            time = timer_timestamp();

            sha256_update_content(commit_generator,__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__,strlen(__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__));
            sha256_update_content(commit_generator,name,strlen(file_hash));
            sha256_update_content(commit_generator,__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__,strlen(__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__));
            sha256_update_content(commit_generator,name,strlen(name));
            sha256_update_content(commit_generator,__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__,strlen(__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__));
            sha256_update_content(commit_generator,time,strlen(time));
        }
    }

}


void track(struct zip* archive, size_t flags,char ***option_values, int *option_counts){
    bool is_archive_being_tracked = is_already_initialized(archive);
    if ((is_archive_being_tracked) && !(flags & (1 << 2))){
        show_message("archive is already being tracked");
        return;
    }

    hash_map* map_path_hash = create_hash_map();

    struct sha256_generator* tree_generator = sha256_create_new_generator();

    struct sha256_generator* commit_generator = sha256_create_new_generator();

    clean_archive(archive,map_path_hash,tree_generator,commit_generator);

    size_t map_size = 0;

    for (int i=0;i<JVC_HASHMAP_SIZE;++i){
        hash_node* node = map_path_hash->buckets[i];
        while(node){
            char* path = blob_get_path(node->value);

            copy_file_inzip_ng(archive,node->key,path);

            node = node->next;

            free(path);
        }
    }

    struct jvc_tree* tree_blob = (struct jvc_tree *) malloc(sizeof(struct jvc_tree));

    tree_blob->id = sha256_string(tree_generator->data,tree_generator->sz);
    tree_blob->map = map_path_hash;

    struct jvc_index* index_content = (struct jvc_index *)malloc(sizeof(struct jvc_index));

    index_content->map = map_path_hash;

    struct jvc_commit* commit_blob = (struct jvc_commit *)malloc(sizeof(struct jvc_commit));

    commit_blob->id = sha256_string(commit_generator->data,commit_generator->sz);
    commit_blob->parent = NULL;
    commit_blob->tree = tree_blob;
    commit_blob->message = option_values['m'-'a'][0];

    commit_add_blob(archive,commit_blob);
    tree_add_blob(archive,tree_blob);
    write_to_file_inzip_ng(archive,__CONSTANTS_RW_BASE__ __CONSTANTS_RW_HEAD__,commit_blob->id,strlen(commit_blob->id));

    tree_free(&tree_blob);
    commit_free(&commit_blob);
    free(&commit_generator);
    free(&tree_generator);
}

/*checks if head and index are already present*/
void show_track_usage(){
    printf("usage: "__TRACK_HELP__"\n\n");
    printf("Following are the available flags and arguments:\n");
    
    for (int i=0;i<sizeof(track_option_names)/sizeof(track_option_names[0]);++i){
        printf("  %-10s: %s\n", track_option_names[i], track_option_descriptions[i]);
    }
}

/*load all the options*/
hash_map* load_options(){
    hash_map* options_map = create_hash_map();
    for (int i=0;i<sizeof(track_option_names)/sizeof(track_option_names[0]);++i){
        char* token = strtok(strdup(track_option_names[i]),"|");
        while(token != NULL){
            //printf("inserting %s and %s\n",token,track_option_descriptions[i]);
            hash_map_insert(options_map,token,track_option_descriptions[i]);
            token = strtok(NULL,"|");
        }
        // hash_map_insert(options_map,track_option_names[i],track_option_descriptions[i]);
    }
    return options_map;
}

void process_track(int argc,char** argv){

    show_message("started process_track");

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
    int* options_sizes[__ARGS_OPTION_TYPES__];

    struct zip* archive;

    int command_flags = 0;

    int args_error_status = 0;

    char* error_message = (char *)malloc(sizeof(char)*1000);

    int zip_open_error = 0;

    processArgs(argc,argv, &flags, sizeof(flags)/sizeof(flags[0]), &valargs, sizeof(valargs)/sizeof(valargs[0]), &archive, &zip_open_error, &command_flags, options_array, options_sizes, &args_error_status, &error_message);

    if (((command_flags) && (1<<2)) == 1){
        show_track_usage();
        return;
    } else if (args_error_status != 0){
        show_message("%s",error_message);
        show_track_usage();
        return;
    } else if (zip_open_error == ZIP_ER_NOENT){
        show_message("error: the archive %s not found",argv[2]);
        return;
    } else if (zip_open_error == ZIP_ER_NOZIP){
        show_message("error: %s is not a valid archive",argv[2]);
        return;
    } else if (zip_open_error == ZIP_ER_OPEN){
        show_message("error: cannot open the file %s",argv[2]);
        return;
    } else if (zip_open_error > 0){
        show_message("error: unknown error occurred");
        return;
    }

    track(archive,command_flags,options_array,options_sizes);

    zip_close(archive);
    return;
}