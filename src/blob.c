#include "../include/blob.h"

char* blob_get_path(const char* blob_id){
    size_t blob_path_size = strlen(JVC_BASE) + strlen(JVC_BLOBS) + strlen(blob_id) + 1;

    size_t sz = 0;

    char* blob_path = (char *)malloc(sizeof(char) * blob_path_size);

    memcpy(blob_path + sz,JVC_BASE,strlen(JVC_BASE));

    sz += strlen(JVC_BASE);

    memcpy(blob_path + sz,JVC_BLOBS,strlen(JVC_BLOBS));

    sz += strlen(JVC_BLOBS);

    memcpy(blob_path + sz,blob_id,2);

    sz += 2;

    memcpy(blob_path + sz,"/",1);

    sz += 1;

    memcpy(blob_path + sz,blob_id+2,strlen(blob_id+2));

    sz += strlen(blob_id+2);

    *(blob_path + sz) = '\0';

    return blob_path;
}