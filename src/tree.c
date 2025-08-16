#include "../include/tree.h"

void tree_add_blob(struct zip* archive, struct jvc_tree* tree){
    char* blob_path = blob_get_path(tree->id);

    size_t sz = strlen((const char *) blob_path) + strlen(__CONSTANTS_RW_BLOBTYPE_IDENTIFIER__) + 1;

    size_t blob_type_index = 0;

    char* blob_type_path = (char *) malloc(sizeof(char) * sz);

    std_append_chars(blob_type_path,&blob_type_index,blob_path,strlen(blob_path));

    std_append_chars(blob_type_path,&blob_type_index,__CONSTANTS_RW_BLOBTYPE_IDENTIFIER__,strlen(__CONSTANTS_RW_BLOBTYPE_IDENTIFIER__));

    // blob_type_path[0] = '\0';

    // strcat(blob_type_path,blob_path);
    // strcat(blob_type_path,__CONSTANTS_RW_BLOBTYPE_IDENTIFIER__);

    blob_type_path[blob_type_index] = '\0';

    size_t blob_size = 0;
    
    char* blob_content = serialize_hash_map_to_binary(tree->map,&blob_size);
    write_to_file_inzip_ng(archive,blob_path,blob_content,blob_size);

    write_to_file_inzip_ng(archive,blob_type_path,__CONSTANTS_RW_TREE_IDENTIFIER__,strlen(__CONSTANTS_RW_TREE_IDENTIFIER__));
}

struct jvc_tree* tree_get_tree(struct zip* archive,char* id){
    struct jvc_tree* tree = (struct jvc_tree *)malloc(sizeof(struct jvc_tree));
    tree->id = strdup(id);

    char* tree_blob_path = blob_get_path(id);
    size_t buffer_size = 1024;

    char* content = (char *)malloc(sizeof(char)*__CONSTANTS_RW_STRING_BUFFER);

    size_t content_size = 0;

    struct zip_file* file = zip_fopen(archive,tree_blob_path,0);

    while(1 > 0){
        zip_int64_t bytes_read = zip_fread(file,content+content_size,buffer_size);

        //printf("bytes_read: %d\n",bytes_read);

        if (bytes_read == 0){
            break;
        } else if (bytes_read == -1){
            // zip_error_t *zf_error = zip_file_get_error(archive);
            // fprintf(stderr, "zip_fread error: %s\n", zip_error_strerror(zf_error));
            break;
        } else{
            content_size += (size_t) bytes_read;
        }
    }

    zip_fclose(file);

    *(content + content_size) = '\0';

    hash_map* map = deserialize_hash_map_from_binary(content,content_size);

    tree->map = map;

    __RW_MEMFREE__(tree_blob_path);
    __RW_MEMFREE__(content);

    return tree;
}

void tree_free(struct jvc_tree** tree){
    if (!tree && !(*tree)) return;
    
    __RW_MEMFREE__((*tree)->id);
    free_hash_map((*tree)->map);

    __RW_MEMFREE__(*tree);
    tree = NULL;
}

struct hash_map* tree_getworktree(struct zip* archive){
    struct hash_map* worktree = (struct hash_map*) malloc(sizeof(struct hash_map));

    zip_uint64_t num_entries = (zip_uint64_t) zip_get_num_entries(archive, 0);

    for (zip_uint64_t i = 0; i < num_entries; i++){
        const char *name = zip_get_name(archive, i,  ZIP_FL_UNCHANGED);

        if (name && strncmp(name, __CONSTANTS_RW_BASE__, strlen(__CONSTANTS_RW_BASE__)) == 0){
            continue;
        } else{
            zip_file_t* file = zip_fopen(archive,name,0);
            char* file_hash = sha256_zip_file_ng(file);

            hash_map_insert(worktree,name,file_hash);

            __RW_MEMFREE__(file_hash);

            zip_fclose(file);
        }
    }
    return worktree;
}