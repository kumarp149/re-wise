#include "../include/sha256.h"

char* sha256_string(const char* str, size_t sz) {
    if (!str || sz <= 0) {
        return NULL;
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;

    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str, sz);
    SHA256_Final(hash, &sha256);

    char* hash_string = malloc(SHA256_DIGEST_LENGTH * 2 + 1);
    if (!hash_string) {
        return NULL;
    }

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hash_string + (i * 2), "%02x", hash[i]);
    }
    hash_string[SHA256_DIGEST_LENGTH * 2] = '\0';

    return hash_string;
}

struct sha256_generator* sha256_create_new_generator(){
    struct sha256_generator* generator = (struct sha256_generator*) malloc(sizeof(struct sha256_generator));

    generator->data = (char *)malloc(sizeof(char)*__CONSTANTS_HASH_GENERATOR_SIZE__);

    generator->sz = 0;

    sha256_update_content(generator,__CONSTANTS_RW_HASH_GENERATOR_PREFIX__,strlen(__CONSTANTS_RW_HASH_GENERATOR_PREFIX__));

    return generator;
}

void sha256_update_content(struct sha256_generator* generator,const char* content,size_t sz){
    size_t bytes_left = __CONSTANTS_HASH_GENERATOR_SIZE__ - generator->sz;
    if (bytes_left <= 0){
        return;
    } else if (bytes_left >= sz){
        memcpy(generator->data + generator->sz,content,sz);
        generator->sz += sz;
    } else{
        memcpy(generator->data + generator->sz,content,bytes_left);
        generator->sz += bytes_left;
    }
    return;
}

char* sha256_get(struct sha256_generator* generator){
    return sha256_string(generator->data,generator->sz);
}

void sha256_free(struct sha256_generator** generator){
    if (generator != NULL && *generator != NULL){
        free((*generator)->data);
        (*generator)->data = NULL;

        free(*generator);
        *generator = NULL;
    }
}