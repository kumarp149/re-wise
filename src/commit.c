#include "../include/commit.h"

void show_commit_usage(struct args_flag* flags,size_t flags_size,struct args_valarg* valargs,size_t valargs_size){
    show_message("usage: " __COMMIT_HELP__ "\n");
    show_message("Following are the available flags and arguments:");

    for (size_t i=0;i<flags_size;++i){
        show_message("  %s|%s: %s",(flags+i)->shortId,(flags+i)->longId,(flags+i)->long_description);
    }
    for (size_t i=0;i<valargs_size;++i){
        show_message("  %s|%s: %s",(valargs+i)->shortId,(valargs+i)->longId,(valargs+i)->long_description);
    }
}

void commit_append_tree(char* blob_content,struct jvc_commit* commit,size_t* sz){
    memcpy(blob_content + *sz,__COMMIT_TREE_PREFIX__,strlen(__COMMIT_TREE_PREFIX__));

    *sz += strlen(__COMMIT_TREE_PREFIX__);

    memcpy(blob_content + *sz,commit->tree->id,strlen(commit->tree->id));

    *sz += strlen(commit->tree->id);

    memcpy(blob_content + *sz,"\n",1);

    *sz += 1;
}

void commit_append_parent(char* blob_content,struct jvc_commit* commit,size_t* sz){
    memcpy(blob_content + *sz,__COMMIT_PARENT_PREFIX__,strlen(__COMMIT_PARENT_PREFIX__));

    *sz += strlen(__COMMIT_PARENT_PREFIX__);

    if (commit->parent != NULL){
        memcpy(blob_content + *sz,commit->parent->id,strlen(commit->parent->id));
        *sz += strlen(commit->parent->id);
    }

    memcpy(blob_content + *sz,"\n",1);
    *sz += 1;
}

void commit_append_message(char* blob_content,struct jvc_commit* commit,size_t* sz){
    memcpy(blob_content + *sz,__COMMIT_MESSAGE_PREFIX__,strlen(__COMMIT_MESSAGE_PREFIX__));

    *sz += strlen(__COMMIT_MESSAGE_PREFIX__);

    memcpy(blob_content + *sz,commit->message,strlen(commit->message));

    *sz += strlen(commit->message);

    memcpy(blob_content + *sz,"\n",1);
    *sz += 1;
}

void commit_add_blob(struct zip* archive,struct jvc_commit* commit){

    char* blob_path = blob_get_path(commit->id);

    size_t sz = strlen((const char *) blob_path) + strlen(__CONSTANTS_RW_BLOBTYPE_IDENTIFIER__);

    char* blob_type_path = (char *) malloc(sizeof(char) * sz);

    blob_type_path[0] = '\0';

    strcat(blob_type_path,blob_path);
    strcat(blob_type_path,__CONSTANTS_RW_BLOBTYPE_IDENTIFIER__);

    blob_type_path[sz] = '\0';

    char* blob_content = (char *) malloc(sizeof(char)*__CONSTANTS_RW_STRING_BUFFER);

    size_t size_filled = 0;

    commit_append_tree(blob_content,commit,&size_filled);
    commit_append_parent(blob_content,commit,&size_filled);
    commit_append_message(blob_content,commit,&size_filled);

    *(blob_content+size_filled) = '\0';

    write_to_file_inzip_ng(archive,blob_path,blob_content,size_filled);

    write_to_file_inzip_ng(archive,blob_type_path,__CONSTANTS_RW_COMMIT_IDENTIFIER__,strlen(__CONSTANTS_RW_COMMIT_IDENTIFIER__));

    __RW_MEMFREE__(blob_type_path);
}

char* commit_get_head_commit(struct zip* archive){
    char* head_commit_id = (char *)malloc(sizeof(char)*65);

    struct zip_file* head_file = zip_fopen(archive,__CONSTANTS_RW_BASE__ __CONSTANTS_RW_HEAD__,0);

    zip_fread(head_file,head_commit_id,64);

    *(head_commit_id + 64) = '\0';

    zip_fclose(head_file);

    return head_commit_id;
}

struct jvc_commit* commit_get_commit(struct zip* archive,char *id){
    char* commit_blob_path = blob_get_path(id);
    char* line = (char *)malloc(sizeof(char)*__CONSTANT_RW_STRING_BUFFER_M);
    char* prefix = (char *)malloc(sizeof(char)*__CONSTANT_RW_STRING_BUFFER_S);

    size_t line_size = 0;
    size_t prefix_size = 0;
    struct zip_file* file = zip_fopen(archive,commit_blob_path,0);

    struct jvc_commit* commit = (struct jvc_commit *)malloc(sizeof(struct jvc_commit));

    commit->id = strdup(id);

    char ch;
    bool space_found = false;
    while (zip_fread(file, &ch, 1) == 1){
        if (ch == '\n'){
            *(line + line_size) = '\0';
            if (strcmp(prefix,__COMMIT_TREE_PREFIX__) == 0){
                commit->tree = tree_get_tree(archive,line);
            } else if (strcmp(prefix,__COMMIT_PARENT_PREFIX__) == 0){
                if (line_size == 0){
                    commit->parent = NULL;
                } else{
                    commit->parent = (struct jvc_commit *)malloc(sizeof(struct jvc_commit));
                    commit->parent->id = strdup(line);
                }
            } else if (strcmp(prefix,__COMMIT_MESSAGE_PREFIX__) == 0){
                commit->message = strdup(line);
            }

            __RW_MEMFREE__(line);
            __RW_MEMFREE__(prefix);

            line = (char *)malloc(sizeof(char)*__CONSTANT_RW_STRING_BUFFER_M);
            prefix = (char *)malloc(sizeof(char)*__CONSTANT_RW_STRING_BUFFER_S);

            line_size = 0;
            prefix_size = 0;

            space_found = false;
        } else if (ch == ' '){
            space_found = true;
            *(prefix + prefix_size) = ch;
            prefix_size++;
            *(prefix + prefix_size) = '\0';
            continue;
        } else{
            if (space_found == true){
                *(line + line_size) = ch;
                line_size++;
            } else{
                *(prefix + prefix_size) = ch;
                prefix_size++;
            }
        }
    }

    __RW_MEMFREE__(line);
    __RW_MEMFREE__(prefix);

    zip_fclose(file);

    return commit;
}

void create_new_commit(struct zip* archive,char ***option_values,int command_flags){
    char* head_commit_id = commit_get_head_commit(archive);
    
    struct jvc_commit* head_commit = commit_get_commit(archive,head_commit_id);

    hash_map* head_map = head_commit->tree->map;

    bool is_archived_changed = false;

    hash_map* path_map = create_hash_map();
    struct sha256_generator* tree_generator = sha256_create_new_generator();
    struct sha256_generator* commit_generator = sha256_create_new_generator();

    zip_int64_t num_entries = zip_get_num_entries(archive, 0);

    for (zip_int64_t i = num_entries - 1; i >= 0; i--){
        const char *name = zip_get_name(archive, (zip_uint64_t) i, 0);

        if (name && strncmp(name, __CONSTANTS_RW_BASE__, strlen(__CONSTANTS_RW_BASE__)) == 0){
            continue;
        } else if (name){
            zip_file_t* file = zip_fopen(archive,name,0);
            char* file_hash = sha256_zip_file_ng(file);

            char* file_hash_inhead = hash_map_get(head_map,name);

            if ((file_hash_inhead == NULL) || (strcmp(file_hash_inhead,file_hash) != 0)){
                is_archived_changed = true;
            }
            
            hash_map_insert(path_map,name,file_hash);
            zip_fclose(file);

            char* time = timer_timestamp();

            free(time);

            sha256_update_content(tree_generator,__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__,strlen(__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__));
            sha256_update_content(tree_generator,name,strlen(file_hash));
            sha256_update_content(tree_generator,__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__,strlen(__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__));
            sha256_update_content(tree_generator,time,strlen(time));

            time = timer_timestamp();

            free(time);

            sha256_update_content(commit_generator,__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__,strlen(__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__));
            sha256_update_content(commit_generator,name,strlen(file_hash));
            sha256_update_content(commit_generator,__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__,strlen(__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__));
            sha256_update_content(commit_generator,name,strlen(name));
            sha256_update_content(commit_generator,__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__,strlen(__CONSTANTS_RW_HASH_GENERATOR_DELIMITER__));
            sha256_update_content(commit_generator,time,strlen(time));
        }
    }

    for (int i=0;i<JVC_HASHMAP_SIZE;++i){
        hash_node* node = path_map->buckets[i];

        while(node){
            char* path = blob_get_path(node->value);

            size_t sz = strlen((const char *) path) + strlen(__CONSTANTS_RW_BLOBTYPE_IDENTIFIER__);

            char* blob_type_path = (char *) malloc(sizeof(char) * sz);

            blob_type_path[0] = '\0';

            strcat(blob_type_path,path);

            strcat(blob_type_path,__CONSTANTS_RW_BLOBTYPE_IDENTIFIER__);

            blob_type_path[sz] = '\0';

            copy_file_inzip_ng(archive,node->key,path);

            write_to_file_inzip_ng(archive,blob_type_path,__CONSTANTS_RW_BLOB_IDENTIFIER__,strlen(__CONSTANTS_RW_BLOB_IDENTIFIER__));

            node = node->next;

            free(path);
        }
    }

    if (is_archived_changed == false && (command_flags & (1<<__COMMIT_FLAGBIT_EMPTY_)) == 0){
        show_message("no changes to commit");
        return;
    }

    struct jvc_tree* tree_blob = (struct jvc_tree *) malloc(sizeof(struct jvc_tree));

    tree_blob->id = sha256_string(tree_generator->data,tree_generator->sz);
    tree_blob->map = path_map;

    struct jvc_commit* commit_blob = (struct jvc_commit *)malloc(sizeof(struct jvc_commit));

    struct jvc_commit* parent_commit = (struct jvc_commit *)malloc(sizeof(struct jvc_commit));

    parent_commit->id = commit_get_head_commit(archive);

    commit_blob->id = sha256_string(commit_generator->data,commit_generator->sz);
    commit_blob->parent = parent_commit;
    commit_blob->tree = tree_blob;
    commit_blob->message = option_values['m'-'a'][0];

    commit_add_blob(archive,commit_blob);
    tree_add_blob(archive,tree_blob);
    write_to_file_inzip_ng(archive,__CONSTANTS_RW_BASE__ __CONSTANTS_RW_HEAD__,commit_blob->id,strlen(commit_blob->id));

    show_message("commit %s created",commit_blob->id);
}

void process_commit(int argc,char** argv){
    struct args_flag flags[] = {
        #define X(longId, shortId, short_description, long_description, flagId) { longId, shortId, short_description, long_description, flagId },
            __COMMIT_FLAGS__
        #undef X
    };

    struct args_valarg valargs[] = {
        #define X(mandatory, longId, shortId, short_description, long_description, maxCount) {mandatory, longId, shortId, short_description, long_description, maxCount },
            __COMMIT_ARGS__
        #undef X
    };

    struct zip* archive;

    int command_flags = 0;

    char** options_array[__ARGS_OPTION_TYPES__];
    int options_sizes[__ARGS_OPTION_TYPES__] = {0};

    int proceed_further;

    processArgs(argc,argv,flags,sizeof(flags)/sizeof(flags[0]),valargs,sizeof(valargs)/sizeof(valargs[0]),&archive,&command_flags,options_array, options_sizes,show_commit_usage,&proceed_further,__COMMIT_FLAGBIT_HELP_);

    if (proceed_further != 1) return;

    create_new_commit(archive,options_array,command_flags);

    zip_close(archive);
    
    return;
}

bool commit_is_valid(struct zip* archive,char* commit_id){
    char* blob_path = blob_get_path(commit_id);
    char* blob_path_type = blob_get_type_path(commit_id);

    if (!file_exists_inzip_ng(archive,blob_path) || !(file_exists_inzip_ng(archive,blob_path_type))){
        __RW_MEMFREE__(blob_path);
        __RW_MEMFREE__(blob_path_type);

        return false;
    }

    char* blob_type = read_from_file_inzip_ng(archive,(const char*) blob_path_type);

    if (strcmp(blob_type,__CONSTANTS_RW_COMMIT_IDENTIFIER__) == 0){
        __RW_MEMFREE__(blob_path);
        __RW_MEMFREE__(blob_path_type);
        __RW_MEMFREE__(blob_type);

        return true;
    } else{
        __RW_MEMFREE__(blob_path);
        __RW_MEMFREE__(blob_path_type);
        __RW_MEMFREE__(blob_type);

        return false;
    }
}

char* commit_resolve_commit(struct zip* archive,char *identifier,char** message){
    if (strcmp(identifier,__CONSTANTS_RW_HEAD_IDENTIFIER_) == 0) return commit_get_head_commit(archive);

    if (strlen(identifier) < 3){
        *message = "too less characters given to identify the commit";
        return NULL;
    }

    size_t sz = strlen(__CONSTANTS_RW_BASE__) + strlen(__CONSTANTS_RW_BLOBS__) + strlen(identifier) + 1;

    size_t index = 0;

    char* identifier_prefix = (char *)malloc(sizeof(char) * sz);

    memcpy(identifier_prefix + index,__CONSTANTS_RW_BASE__,strlen(__CONSTANTS_RW_BASE__));

    index += strlen(__CONSTANTS_RW_BASE__);

    memcpy(identifier_prefix + index,__CONSTANTS_RW_BLOBS__,strlen(__CONSTANTS_RW_BLOBS__));

    index += strlen(__CONSTANTS_RW_BLOBS__);

    memcpy(identifier_prefix + index,identifier,2);

    index += 2;

    identifier_prefix[index] = '/';

    index++;

    memcpy(identifier_prefix + index,identifier+2,strlen(identifier)-2);

    index += strlen(identifier)-2;

    identifier_prefix[index] = '\0';

    int count = 0;
    
    zip_uint64_t num_entries = (zip_uint64_t) zip_get_num_entries(archive, 0);

    size_t prefix_len = strlen(identifier_prefix);
    
    char* commit_blob_path;

    for (zip_uint64_t i = 0; i < num_entries; ++i){
        const char* name = zip_get_name(archive, i, ZIP_FL_ENC_GUESS);
        if (!name || strstr(name,__CONSTANTS_RW_BLOBTYPE_IDENTIFIER__) != NULL) {
            continue;
        }

        if (strncmp(name, __CONSTANTS_RW_BASE__ __CONSTANTS_RW_BLOBS__, strlen(__CONSTANTS_RW_BASE__ __CONSTANTS_RW_BLOBS__)) != 0){
            continue;
        }

        char* object_id = blob_get_commitid((char *)name);

        if (strncmp((const char *)name, identifier_prefix, prefix_len) == 0 && commit_is_valid(archive,object_id) == true){
            count++;
            commit_blob_path = strdup(name);
        }

        if (count > 1) break;
    }

    __RW_MEMFREE__(identifier_prefix);

    if (count == 0){
        *message = "no commit found with the given prefix";
        return NULL;
    }
    if (count > 1){
        *message = "too many commits found with the given prefix";
        return NULL;
    }

    char* commit_id = blob_get_commitid(commit_blob_path);

    return commit_id;
}

char* commit_get_parent_commit_id(struct zip* archive,char *commit_id){
    struct jvc_commit* commit = commit_get_commit(archive,commit_id);

    if (commit->parent) return commit->parent->id;
    return NULL;
}