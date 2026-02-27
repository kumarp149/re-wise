#include "../include/blob.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int build_header(struct copy_ctx *ctx) {
    size_t id_len = strlen(ctx->blob->id);

    ctx->header_len = 1 + 2 + id_len + 2;
    ctx->header = malloc(ctx->header_len);
    if (!ctx->header)
        return -1;

    size_t off = 0;

    ctx->header[off++] = (char)(ctx->blob->type + '0');
    memcpy(ctx->header + off, __CONSTANTS_RW_NEWLINE__, 2);
    off += 2;

    memcpy(ctx->header + off, ctx->blob->id, id_len);
    off += id_len;

    memcpy(ctx->header + off, __CONSTANTS_RW_NEWLINE__, 2);

    return 0;
}

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

static zip_int64_t copy_callback(void *ud,
                                 void *data,
                                 zip_uint64_t len,
                                 zip_source_cmd_t cmd)
{
    struct copy_ctx *ctx = (struct copy_ctx *)ud;

    switch (cmd) {

    case ZIP_SOURCE_OPEN:
        ctx->header_offset = 0;
        ctx->content_offset = 0;
        ctx->header_done = 0;

        if (ctx->header == NULL) {
            if (build_header(ctx) < 0)
                return -1;
        }

        return 0;

    case ZIP_SOURCE_READ: {
        if (len == 0)
            return 0;

        char *out = (char *)data;
        zip_uint64_t written = 0;

        /* ---- Emit header first ---- */
        if (!ctx->header_done) {
            size_t remaining = ctx->header_len - ctx->header_offset;
            size_t to_copy = remaining < len ? remaining : len;

            memcpy(out,
                   ctx->header + ctx->header_offset,
                   to_copy);

            ctx->header_offset += to_copy;
            written += to_copy;

            if (ctx->header_offset == ctx->header_len)
                ctx->header_done = 1;

            return (zip_int64_t) written;
        }

        /* ---- Emit content ---- */

        /* Memory blob */
        if (ctx->blob->type == __BLOB_COMMITTYPE__ ||
            ctx->blob->type == __BLOB_TREETYPE__) {

            if (!ctx->blob->content)
                return 0;

            if (ctx->content_len == 0)
                ctx->content_len = strlen(ctx->blob->content);

            size_t remaining =
                ctx->content_len - ctx->content_offset;

            if (remaining == 0)
                return 0;

            size_t to_copy =
                remaining < len ? remaining : len;

            memcpy(out,
                   ctx->blob->content + ctx->content_offset,
                   to_copy);

            ctx->content_offset += to_copy;
            return (zip_int64_t) to_copy;
        }

        /* File blob (streaming) */
        if (ctx->zf) {
            zip_uint64_t to_read =
                len < __BLOB_CHUNK_SIZE__ ?
                len : __BLOB_CHUNK_SIZE__;

            zip_int64_t n =
                zip_fread(ctx->zf, out, to_read);

            if (n < 0)
                return -1;

            return n;  // 0 means EOF
        }

        return 0;
    }

    case ZIP_SOURCE_CLOSE:
        return 0;

    case ZIP_SOURCE_STAT: {
        struct zip_stat *st = (struct zip_stat *)data;
        zip_stat_init(st);
        return sizeof(*st);
    }

    case ZIP_SOURCE_ERROR:
        return 0;

    case ZIP_SOURCE_FREE:
        if (ctx->zf)
            zip_fclose(ctx->zf);

        if (ctx->header)
            free(ctx->header);

        free(ctx);
        return 0;

    default:
        return -1;
    }
}
void blob_write_blob(struct blob *blob, struct zip *archive)
{
    struct copy_ctx *ctx =
        malloc(sizeof(struct copy_ctx));
    if (!ctx)
        exit(1);

    memset(ctx, 0, sizeof(*ctx));
    ctx->blob = blob;

    if (blob->type == __BLOB_OBJECTTYPE__) {

        zip_int64_t idx =
            zip_name_locate(archive, blob->src, 0);

        if (idx < 0) {
            free(ctx);
            exit(1);
        }

        ctx->zf =
            zip_fopen_index(archive,
                            (zip_uint64_t)idx,
                            0);

        if (!ctx->zf) {
            free(ctx);
            exit(1);
        }
    }

    zip_source_t *zs =
        zip_source_function_create(copy_callback,
                                   ctx,
                                   NULL);

    if (!zs) {
        if (ctx->zf)
            zip_fclose(ctx->zf);
        free(ctx);
        exit(1);
    }

    if (zip_file_add(archive,
                     blob_get_path(blob->id),
                     zs,
                     ZIP_FL_OVERWRITE |
                     ZIP_FL_ENC_UTF_8) < 0) {

        zip_source_free(zs);
        return;
    } else{
        log_message("file created at: %s",blob_get_path(blob->id));
    }

    zip_fclose(ctx->zf);

    /* DO NOT call zip_source_close */
    /* DO NOT free ctx */
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