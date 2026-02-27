#ifndef __UTILS__
#define __UTILS__

#include "zip.h"
#include "openssl/sha.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "zlib.h"
#include "global.h"
#include "constants.h"
#include <stdbool.h>
#include "structs/hash-map.h"
#include "print.h"
#include "blob.h"

#define __BUFFERSIZE_OBJECT_TO_WORKTREE__ 1024

struct object_to_worktree_ctx{
    char* object_id;
    zip_file_t *zf;
    bool eof;
    int blob_type;
};

/*get the sha256 hash of a file in zip*/
char* sha256_zip_file(const char* zip_filename, const char* file_in_zip);

/*takes an already opened file and gives the sha256 of it*/
char* sha256_zip_file_ng(struct zip_file* file);

/*copy a file from one path to another path in zip*/
void copy_file_inzip(const char* zip_filename,const char* file_from,const char* file_to);

void copy_file_inzip_ng(zip_t* archive,const char* file_from,const char* file_to);

/*delete files in zip*/
void delete_files_inzip(const char* zip_filename, const char** file_paths, size_t num_files);

void delete_files_inzip_ng(struct zip* archive,const char** file_paths,size_t num_files);

/*writes content to a file in zip*/
void write_to_file_inzip(const char* zip_filename, const char* file_path, const char* content,size_t sz);

/*takes an already initialized archive and writes the content to it*/
void write_to_file_inzip_ng(struct zip* archive,char* file_path,char* content,size_t sz);


/*takes the already initialized archive and checks if the file exists*/
bool file_exists_inzip_ng(struct zip* archive,const char* file_path);

void delete_folder_inzip_ng(struct zip* archive,const char* folder_path);

hash_map* iterate_zip(struct zip* archive);

char* read_from_file_inzip_ng(zip_t* archive, const char* file_path);

/*copy a file from worktree*/
void copy_file_from_worktree(struct zip* archive,char* path);

/*copy the object file to worktree*/
void copy_object_file(struct zip* archive,char* obj_id,char* path);

#endif