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

void commit_append_message(char* blob_content,struct jvc_commit* commit,size_t* sz){
    memcpy(blob_content + *sz,__COMMIT_MESSAGE_PREFIX__,strlen(__COMMIT_MESSAGE_PREFIX__));

    *sz += strlen(__COMMIT_MESSAGE_PREFIX__);

    memcpy(blob_content + *sz,commit->message,strlen(commit->message));

    *sz += strlen(commit->message);

    memcpy(blob_content + *sz,"\n",1);
    *sz += 1;

    return;
}

void commit_add_blob(struct zip* archive,struct jvc_commit* commit){
    //printf("adding the blob\n");

    char* blob_path = blob_get_path(commit->id);

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
    commit_append_message(blob_content,commit,&size_filled);

    *(blob_content+size_filled) = '\0';

    //printf("size_filled: %d\n",size_filled);
    //printf("blob_content length: %d\n",strlen(blob_content));

    //printf("added commit message\n");

    write_to_file_inzip_ng(archive,blob_path,blob_content,size_filled);

    // printf("added commit blob: %s\n",commit->id);
    // printf("commit blob content: %s\n",blob_content);
}

char* commit_get_head_commit(struct zip* archive){
    char* head_commit_id = (char *)malloc(sizeof(char)*65);

    struct zip_file* head_file = zip_fopen(archive,JVC_BASE JVC_HEAD,0);

    size_t bytes_read = zip_fread(head_file,head_commit_id,64);

    *(head_commit_id + 64) = '\0';

    return head_commit_id;
}

struct jvc_commit* commit_get_commit(struct zip* archive,char *id){
    char* commit_blob_path = blob_get_path(id);

    //printf("calling commit_get_commit for %s\n",id);

    char* line = (char *)malloc(sizeof(char)*1001);
    char* prefix = (char *)malloc(sizeof(char)*100);

    size_t line_size = 0;
    size_t prefix_size = 0;
    struct zip_file* file = zip_fopen(archive,commit_blob_path,0);

    struct jvc_commit* commit = (struct jvc_commit *)malloc(sizeof(struct jvc_commit));

    char ch;
    bool space_found = false;
    while (zip_fread(file, &ch, 1) == 1){
        if (ch == '\n'){
            //printf("line break found. line: %s and prefix: %s\n",line,prefix);
            *(line + line_size) = '\0';
            if (strcmp(prefix,__COMMIT_TREE_PREFIX__) == 0){
                //printf("tree found\n");
                commit->tree = tree_get_tree(archive,line);
            } else if (strcmp(prefix,__COMMIT_PARENT_PREFIX__) == 0){
                if (line_size == 0){
                    //printf("parent cannot be found\n");
                    commit->parent = NULL;
                } else{
                    //printf("parent found\n");
                    commit->parent = (struct jvc_commit *)malloc(sizeof(struct jvc_commit));
                    commit->parent->id = line;
                }
            } else if (strcmp(prefix,__COMMIT_MESSAGE_PREFIX__) == 0){
                //printf("message found\n");
                commit->message = line;
                //printf("message found: %s\n",commit->message);
            }

            line = (char *)malloc(sizeof(char)*1000);
            prefix = (char *)malloc(sizeof(char)*100);

            line_size = 0;
            prefix_size = 0;

            space_found = false;
        } else if (ch == ' '){
            space_found = true;
            *(prefix + prefix_size) = ch;
            prefix_size++;
            *(prefix + prefix_size) = '\0';
            //printf("space found. prefix: %s\n",prefix);
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

    return commit;
}