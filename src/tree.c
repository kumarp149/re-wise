#include "../include/tree.h"

void tree_add_blob(struct zip* archive, struct jvc_tree* tree){
    char* blob_path = blob_get_path(tree->id);
    size_t blob_size = 0;
    
    char* blob_content = serialize_hash_map_to_binary(tree->map,&blob_size);
    write_to_file_inzip_ng(archive,blob_path,blob_content,blob_size);

    // if (blob_path){
    //     free(blob_path);
    //     blob_path = NULL;
    // }
}

struct jvc_tree* tree_get_tree(struct zip* archive,char* id){
    struct jvc_tree* tree = (struct jvc_tree *)malloc(sizeof(struct jvc_tree));
    tree->id = id;

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

    return tree;
}

void tree_free(struct jvc_tree** tree){
    if (tree && *tree){
        if ((*tree)->id) free((*tree)->id);
        (*tree)->id = NULL;

        if ((*tree)->map) free_hash_map((*tree)->map);
        *tree = NULL;
    }
}