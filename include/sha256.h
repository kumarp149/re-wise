#ifndef __SHA256__
#define __SHA256__

#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "openssl/sha.h"
#include <string.h>

struct sha256_generator{
    char* data;
    size_t sz;
};

/*get the sha256 of the string. considers upto sz characters*/
char* sha256_string(const char* str, size_t sz);


/*creates a new instance of the hash_generator*/
struct sha256_generator* sha256_create_new_generator();

/*append the first sz characters of the content to hash_generator's data until the hash_generator's data is full*/
void sha256_update_content(struct sha256_generator* generator,const char* content, size_t sz);

/*get the sha256 of the generator*/
char* sha256_get(struct sha256_generator* generator);

/*free the sha256 generator from memory*/
void sha256_free(struct sha256_generator** generator);

#endif