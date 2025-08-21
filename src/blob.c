#include "../include/blob.h"

/*free the memory of the returned pointer after use*/
char* blob_get_path(const char* blob_id){
    size_t blob_path_size = strlen(__CONSTANTS_RW_BASE__) + strlen(__CONSTANTS_RW_BLOBS__) + strlen(blob_id) + 2;

    size_t sz = 0;

    char* blob_path = (char *)malloc(sizeof(char) * blob_path_size);

    std_append_chars(blob_path,&sz,__CONSTANTS_RW_BASE__,strlen(__CONSTANTS_RW_BASE__));

    std_append_chars(blob_path,&sz,__CONSTANTS_RW_BLOBS__,strlen(__CONSTANTS_RW_BLOBS__));

    std_append_chars(blob_path,&sz,(char*) blob_id,2);

    std_append_chars(blob_path,&sz,"/",1);

    std_append_chars(blob_path,&sz,(char*) blob_id+2,strlen(blob_id+2));

    *(blob_path + sz) = '\0';

    return blob_path;
}

char* blob_get_hash_atrevision(struct zip* archive,char* commit_id,char* path){
    struct jvc_commit* commit = commit_get_commit(archive,commit_id);

    return hash_map_get(commit->tree->map,path);
}

char* blob_get_type_path(const char* blob_id){
    char* path = blob_get_path(blob_id);

    size_t sz = strlen(path) + strlen(__CONSTANTS_RW_BLOBTYPE_IDENTIFIER__) + 1;

    char* blob_type_path = (char *) malloc(sizeof(char) * sz);

    size_t index = 0;

    memcpy(blob_type_path + index,path,strlen(path));

    index += strlen(path);

    memcpy(blob_type_path + index,__CONSTANTS_RW_BLOBTYPE_IDENTIFIER__,strlen(__CONSTANTS_RW_BLOBTYPE_IDENTIFIER__));

    index += strlen(__CONSTANTS_RW_BLOBTYPE_IDENTIFIER__);

    *(blob_type_path + index) = '\0';

    __RW_MEMFREE__(path);

    return blob_type_path;
}

char* blob_get_commitid(char *commit_object_path){
    char* occur = strchr(strchr(commit_object_path,'/')+1,'/');
    char* commit_id = (char *)malloc(65);

    size_t index = 0;

    while(*occur != '\0'){
        if (*occur == '/'){
            occur++;
            continue;
        }
        *(commit_id + index) = *occur;
        occur++;
        index++;
    }

    commit_id[index] = '\0';

    return commit_id;
}