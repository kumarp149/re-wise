#include "../include/blob.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* blob_get_path(const char* blob_id){
    size_t blob_path_size = strlen(__CONSTANTS_RW_BASE__) + strlen(__CONSTANTS_RW_BLOBS__) + strlen(blob_id) + 2;

    size_t sz = 0;

    char* blob_path = (char *)malloc(sizeof(char) * blob_path_size);

    if (!blob_path){
        fprintf(stderr,__ERROR_MEMALLOC__);
        exit(1);
    }

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

    if (!path){
        fprintf(stderr,__ERROR_MEMALLOC__);
        exit(1);
    }

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

static zip_int64_t copy_callback(void *ud, void *data, zip_uint64_t len, zip_source_cmd_t cmd) {
    struct copy_ctx *ctx = (struct copy_ctx *)ud;

    size_t bytes_written = 0;

    size_t _temp = 0;

    switch(cmd){
        case ZIP_SOURCE_OPEN:
            /* writes the type of the blob (OBJECT, COMMIT, TREE)*/
            *((char *)data) = ctx->blob->type + '0';
            bytes_written += 1;

            /* writes the new line */
            memcpy(data + bytes_written,__CONSTANTS_RW_NEWLINE__,2);
            bytes_written += 2;

            /* writes the id of the blob (basically sha256 hash of the object or commit or tree)*/
            _temp = strlen(ctx->blob->id);
            memcpy(data + bytes_written,ctx->blob->id,_temp);
            bytes_written += _temp;

            /* writes the new line */
            memcpy(data + bytes_written,__CONSTANTS_RW_NEWLINE__,2);
            bytes_written += 2;

            ctx->eof = 0;
            return (zip_int64_t) bytes_written;

        case ZIP_SOURCE_READ:

            if (ctx->blob->type == __BLOB_COMMITTYPE__ || ctx->blob->type == __BLOB_TREETYPE__){
                if (!ctx->blob->content || ctx->eof > 0) return 0;

                _temp = strlen(ctx->blob->content);
                memcpy(data,ctx->blob->content,_temp);

                bytes_written += _temp;

                __RW_MEMFREE__(ctx->blob->content);

                ctx->eof = 1;

                return (zip_int64_t) ctx->blob->content;
            }

            if (ctx->eof == 1) return 0;
            zip_int64_t n = zip_fread(ctx->zf, data, __BLOB_CHUNK_SIZE__);

            if (n == 0){
                ctx->eof = 1;
                return 0;
            } else if (n < 0){
                return -1;
            }

            return n;

        case ZIP_SOURCE_CLOSE:
            return 0;

        case ZIP_SOURCE_STAT:{
            struct zip_stat* st = (struct zip_stat *)data;
            zip_stat_init(st);
            return sizeof(*st);
        }

        case ZIP_SOURCE_ERROR:
            return 0;

        case ZIP_SOURCE_FREE:
            zip_fclose(ctx->zf);
            __RW_MEMFREE__(ctx);

            return 0;

        default:
            return -1;
    }
    return -1;
}

void blob_write_blob(struct blob* blob,struct zip* archive){
    struct copy_ctx* ctx = (struct copy_ctx *)malloc(sizeof(struct copy_ctx));

    ctx->blob = blob;
    ctx->eof = 0;
    if (blob->type == __BLOB_OBJECTTYPE__){
        zip_int64_t idx = zip_name_locate(archive, ctx->blob->src, 0);
        zip_file_t *zf = zip_fopen_index(archive, (zip_uint64_t) idx, 0);
        ctx->zf = zf;
    }

    zip_source_t *zs = zip_source_function_create(copy_callback, ctx, NULL);
    if (!zs){
        fprintf(stderr, "zip error");
        __RW_MEMFREE__(ctx);
        exit(1);
    }

    if (zip_file_add(archive,blob_get_path(blob->id), zs, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8) < 0){
        fprintf(stderr, "zip error");
        __RW_MEMFREE__(ctx);
        exit(1);
    }
}

int blob_get_type(struct zip* archive,char *id){
    char* blob_path = blob_get_path(id);

    zip_int64_t idx = zip_name_locate(archive, blob_path, 0);
    zip_file_t *zf = zip_fopen_index(archive, (zip_uint64_t) idx, 0);
    
    char* data = (char *)malloc(sizeof(char)*2);
    zip_fread(zf, data, 1);
    data[1] = '\0';

    int res = data[0] - '0';

    __RW_MEMFREE__(data);

    return res;
}