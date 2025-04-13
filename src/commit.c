#include "../include/commit.h"

void commit_append_tree(char* blob_content,struct jvc_commit* commit,size_t* sz){
    memcpy(blob_content + *sz,__COMMIT_TREE_PREFIX__,strlen(__COMMIT_TREE_PREFIX__));

    *sz += strlen(__COMMIT_TREE_PREFIX__);

    memcpy(blob_content + *sz,commit->tree->id,strlen(commit->tree->id));

    *sz += strlen(commit->tree->id);

    memcpy(blob_content + *sz,"\n",1);

    *sz += 1;

    return;
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

    return;
}

void commit_append_mnessage(char* blob_content,struct jvc_commit* commit,size_t* sz){
    memcpy(blob_content + *sz,__COMMIT_MESSAGE_PREFIX__,strlen(__COMMIT_MESSAGE_PREFIX__));

    *sz += strlen(__COMMIT_MESSAGE_PREFIX__);

    memcpy(blob_content + *sz,commit->message,strlen(commit->message));

    *sz += strlen(commit->message);

    return;
}

void commit_add_blob(struct zip* archive,struct jvc_commit* commit){
    printf("adding the blob\n");

    char* blob_path = get_blob_path(commit->id);

    char* blob_content = (char *)malloc(sizeof(char)*JVC_STRING_BUFFER);

    size_t size_filled = 0;

    // memcpy(blob_content+size_filled,__COMMIT_MESSAGE_PREFIX__,strlen(__COMMIT_MESSAGE_PREFIX__));

    // size_filled += strlen(__COMMIT_MESSAGE_PREFIX__);

    // //printf("adding tree id: %s,%d\n",commit->tree->id,strlen(commit->tree->id));

    // memcpy(blob_content+size_filled,commit->tree->id,strlen(commit->tree->id));

    // size_filled += strlen(commit->tree->id);

    // *(blob_content + size_filled) = '\n';
    
    // size_filled++;

    // memcpy(blob_content+size_filled,__COMMIT_TREE_PREFIX__,strlen(__COMMIT_TREE_PREFIX__));

    // size_filled += strlen(__COMMIT_TREE_PREFIX__);

    // if (commit->parent != NULL){
    //     memcpy(blob_content+size_filled,commit->parent->id,strlen(commit->parent->id));
    //     size_filled += strlen(commit->parent->id);
    //     //printf("added parent id\n");
    // } else{
    //     //printf("parent is null\n");
    // }

    // *(blob_content + size_filled) = '\n';

    // size_filled++;

    // memcpy(blob_content+size_filled,commit->message,strlen(commit->message));

    // size_filled += strlen(commit->message);

    commit_append_tree(blob_content,commit,&size_filled);
    commit_append_parent(blob_content,commit,&size_filled);
    commit_append_mnessage(blob_content,commit,&size_filled);

    *(blob_content+size_filled) = '\0';

    printf("size_filled: %d\n",size_filled);
    printf("blob_content length: %d\n",strlen(blob_content));

    //printf("added commit message\n");

    write_to_file_inzip_ng(archive,blob_path,blob_content,size_filled);

    printf("added commit blob: %s\n",commit->id);
    printf("commit blob content: %s\n",blob_content);
}