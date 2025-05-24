#include "../include/utils.h"

/*the methods returns the sha256 hash of the file inside a zip
u must provide it a pointer to an already opened file*/
char* sha256_zip_file_ng(zip_file_t* file){
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    char buffer[4096];
    zip_int64_t bytes_read;
    while ((bytes_read = zip_fread(file, buffer, sizeof(buffer))) > 0) {
        SHA256_Update(&sha256, buffer, (size_t) bytes_read);
    }

    if (bytes_read < 0) {
        //__JVC_ERROR_MESSAGE__ = JVC_UNABLE_TO_READ_FILE;
        zip_fclose(file);
        return NULL;
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    char* hash_string = malloc(SHA256_DIGEST_LENGTH * 2 + 1);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hash_string + (i * 2), "%02x", hash[i]);
    }
    hash_string[SHA256_DIGEST_LENGTH * 2] = '\0';
    return hash_string;
}

char* sha256_zip_file(const char* zip_filename, const char* file_in_zip) {
    //free(__JVC_ERROR_MESSAGE__);

    struct zip* archive = zip_open(zip_filename, ZIP_RDONLY, NULL);
    if (!archive) {
        //__JVC_ERROR_MESSAGE__ = JVC_UNABLE_TO_OPEN_ARCHIVE;
        return NULL;
    }

    struct zip_file* file = zip_fopen(archive, file_in_zip, 0);
    if (!file) {
       // __JVC_ERROR_MESSAGE__ = JVC_UNABLE_TO_OPEN_FILE;
        //zip_close(archive);
        return NULL;
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    char buffer[4096];
    zip_int64_t bytes_read;
    while ((bytes_read = zip_fread(file, buffer, sizeof(buffer))) > 0) {
        size_t read = (size_t) bytes_read;
        SHA256_Update(&sha256, buffer, read);
    }

    if (bytes_read < 0) {
        //__JVC_ERROR_MESSAGE__ = JVC_UNABLE_TO_READ_FILE;
        zip_fclose(file);
        zip_close(archive);
        return NULL;
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    char* hash_string = malloc(SHA256_DIGEST_LENGTH * 2 + 1);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hash_string + (i * 2), "%02x", hash[i]);
    }
    hash_string[SHA256_DIGEST_LENGTH * 2] = '\0';

    zip_fclose(file);
    zip_close(archive);

    return hash_string;
}

void copy_file_inzip_ng(zip_t* archive,const char* file_from,const char* file_to){
    struct zip_file* source_file = zip_fopen(archive, file_from, 0);

    if (!source_file){
        // printf(stderr, "Failed to open source file in zip: %s\n", file_from);
    }

    struct zip_stat stat;
    if (zip_stat(archive, file_from, 0, &stat) != 0){
        fprintf(stderr, "Failed to stat source file in zip: %s\n", file_from);
        zip_fclose(source_file);
    }

    char* buffer = malloc(stat.size);

    if (zip_fread(source_file, buffer, stat.size) != (zip_int64_t) stat.size) {
        fprintf(stderr, "Failed to read source file content\n");
        free(buffer);
        zip_fclose(source_file);
        zip_close(archive);
    }

    zip_fclose(source_file);

    // Add the copied file to the zip archive
    struct zip_source* source = zip_source_buffer(archive, buffer, stat.size, 1);
    if (!source) {
        fprintf(stderr, "Failed to create zip source for file: %s\n", file_to);
        free(buffer);
        zip_close(archive);
    }

    if (zip_file_add(archive, file_to, source, ZIP_FL_OVERWRITE) < 0) {
        fprintf(stderr, "Failed to add file to zip: %s\n", file_to);
        zip_source_free(source);
        zip_close(archive);
    }
    //printf("copied the file from %s to %s\n",file_from,file_to);
}

void copy_file_inzip(const char* zip_filename, const char* file_from, const char* file_to) {
    struct zip* archive = zip_open(zip_filename, ZIP_CHECKCONS, NULL);
    if (!archive) {
        //__JVC_ERROR_MESSAGE__ = JVC_UNABLE_TO_OPEN_ARCHIVE;
        fprintf(stderr, "Failed to open zip file: %s\n", zip_filename);
    }

    // Open the source file in the zip archive
    struct zip_file* source_file = zip_fopen(archive, file_from, 0);
    if (!source_file) {
        //__JVC_ERROR_MESSAGE__ = JVC_UNABLE_TO_OPEN_FILE;
        fprintf(stderr, "Failed to open source file in zip: %s\n", file_from);
        zip_close(archive);
    }

    // Read the source file content
    struct zip_stat stat;
    if (zip_stat(archive, file_from, 0, &stat) != 0) {
        fprintf(stderr, "Failed to stat source file in zip: %s\n", file_from);
        zip_fclose(source_file);
        zip_close(archive);
    }

    char* buffer = malloc(stat.size);
    if (!buffer) {
        fprintf(stderr, "Failed to allocate memory for file content\n");
        zip_fclose(source_file);
        zip_close(archive);
    }

    if (zip_fread(source_file, buffer, stat.size) != (zip_int64_t) stat.size) {
        fprintf(stderr, "Failed to read source file content\n");
        free(buffer);
        zip_fclose(source_file);
        zip_close(archive);
    }

    zip_fclose(source_file);

    // Add the copied file to the zip archive
    struct zip_source* source = zip_source_buffer(archive, buffer, stat.size, 1);
    if (!source) {
        fprintf(stderr, "Failed to create zip source for file: %s\n", file_to);
        free(buffer);
        zip_close(archive);
    }

    if (zip_file_add(archive, file_to, source, ZIP_FL_OVERWRITE) < 0) {
        fprintf(stderr, "Failed to add file to zip: %s\n", file_to);
        zip_source_free(source);
        zip_close(archive);
    }

    // Clean up and close the archive
    zip_close(archive);

    printf("copied");
}

void delete_files_inzip(const char* zip_filename, const char** file_paths, size_t num_files) {
    // Open the zip archive in read-write mode
    struct zip* archive = zip_open(zip_filename, ZIP_CHECKCONS, NULL);
    if (!archive) {
        fprintf(stderr, "Failed to open zip file: %s\n", zip_filename);
        return;
    }

    // Iterate over the list of files to delete
    for (size_t i = 0; i < num_files; i++) {
        const char* file_path = file_paths[i];
        zip_int64_t file_index = zip_name_locate(archive, file_path, 0);

        if (file_index < 0) {
            fprintf(stderr, "File not found in zip: %s\n", file_path);
            continue;
        }

        // Mark the file for deletion
        if (zip_delete(archive, (zip_uint64_t) file_index) != 0) {
            fprintf(stderr, "Failed to delete file from zip: %s\n", file_path);
        } else {
            printf("Deleted file from zip: %s\n", file_path);
        }
    }
    
    // Close the zip archive
    if (zip_close(archive) != 0) {
        fprintf(stderr, "Failed to save changes to zip file: %s\n", zip_filename);
    }
}

void delete_files_inzip_ng(struct zip* archive,const char** file_paths,size_t num_files){
    for (size_t i=0;i<num_files;++i){
        const char* path = file_paths[i];

        zip_int64_t index = zip_name_locate(archive,path,0);
        
        zip_delete(archive,(zip_uint64_t) index);
    }
}

void write_to_file_inzip(const char* zip_filename, const char* file_path, const char* content,size_t sz){
    printf("calling write_to_file_inzip");
    struct zip* archive = zip_open(zip_filename, ZIP_CREATE | ZIP_CHECKCONS, NULL);

    if (!archive){

    }

    struct zip_source* source = zip_source_buffer(archive, content, sz, 0);

    if (!source){
        //
    }

    zip_int64_t file_index = zip_name_locate(archive,file_path,0);

    if (file_index >= 0){
        if (zip_file_replace(archive,(zip_uint64_t) file_index,source,ZIP_FL_OVERWRITE) < 0){
            //
            return;
        }
    } else{
        if (zip_file_add(archive,file_path,source,ZIP_FL_OVERWRITE) < 0){
            //
            return;
        }
    }

    zip_close(archive);
}


void write_to_file_inzip_ng(struct zip* archive,char* file_path,char* content,size_t sz){
    //printf("calling write_to_file_inzip_ng\n");
    struct zip_source* source = zip_source_buffer(archive, content, sz, 0);

    // printf("adding file: %s\n",file_path);

    // printf("source created\n");

    if (!source){
        log_message("unable to create zip source\n");
    }

    if (zip_file_add(archive,file_path,source,ZIP_FL_OVERWRITE) < 0){
        log_message("error adding file: %s\n",file_path);
        return;
    } else{
        log_message("File added to zip successfully: %s\n",file_path);
    }

    // printf("added content to file: %s\n",file_path);

    zip_source_close(source);
}

bool file_exists_inzip_ng(struct zip* archive,const char* file_path){
    zip_int64_t file_index = zip_name_locate(archive, file_path, 0);

    if (file_index < 0){
        return false;
    }
    return true;
}

bool file_exists_in_zip(const char* zip_filename, const char* file_path) {
    // Open the zip archive in read-only mode
    struct zip* archive = zip_open(zip_filename, ZIP_RDONLY, NULL);
    if (!archive) {
        fprintf(stderr, "Failed to open zip file: %s\n", zip_filename);
        return false;
    }

    // Check if the file exists in the zip archive
    zip_int64_t file_index = zip_name_locate(archive, file_path, 0);
    if (file_index < 0) {
        // File not found
        zip_close(archive);
        return false;
    }

    // File found
    zip_close(archive);
    return true;
}

void delete_folder_inzip_ng(struct zip* archive,const char* folder_path){
    zip_int64_t num_entries = zip_get_num_entries(archive, 0);

    for (zip_int64_t i = num_entries - 1; i >= 0; i--) {
        const char *name = zip_get_name(archive, (zip_uint64_t) i, 0);
        if (name && strncmp(name, folder_path, strlen(folder_path)) == 0) {
            if (zip_delete(archive, (zip_uint64_t) i) < 0) {
                log_message( "Failed to delete entry: %s\n", name);
            }
        }
    }
}

hash_map* iterate_zip(struct zip* archive){
    zip_int64_t num_entries = zip_get_num_entries(archive, 0);

    hash_map* map_current_state = create_hash_map();

    for (zip_int64_t i=0;i<num_entries;++i){
        const char *name = zip_get_name(archive, (zip_uint64_t) i, 0);

        if (name && strncmp(name, __CONSTANTS_RW_BASE__, strlen(__CONSTANTS_RW_BASE__)) == 0){
            continue;
        } else{
            zip_file_t* file = zip_fopen(archive,name,0);
            char* file_hash = sha256_zip_file_ng(file);
            hash_map_insert(map_current_state,name,file_hash);
            
            zip_fclose(file);
        }
    }

    return map_current_state;
}