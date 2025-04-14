#ifndef __STRUCTURES__
#define __STRUCTURES__

#define JVC_HASHMAP_SIZE 1024
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct jvc_index{
    struct hash_map* map;
};

struct jvc_head{
    char* id;
};

struct jvc_blob{
    char* id;
    char* data;
};

typedef struct hash_node {
    char *key;
    char *value;
    struct hash_node *next;
} hash_node;

typedef struct hash_map{
    hash_node* buckets[JVC_HASHMAP_SIZE];
} hash_map;

unsigned int hash(const char* key);

/*creates an instance of a hash map*/
hash_map* create_hash_map();

/*inserts a key/value pair to hash_map*/
void hash_map_insert(hash_map* map,const char* key, const char* value);

/*gets the value of a key in the hashmap. returns NULL if the key doesnot exist*/
char* hash_map_get(hash_map* map,const char* key);

/*frees the memory allocated for the hash_map*/
void free_hash_map(hash_map* map);

char* serialize_hash_map_to_binary(hash_map *map, size_t *binary_size);

hash_map* deserialize_hash_map_from_binary(const char *binary_data, size_t binary_size);

void map_get_difference(hash_map* first, hash_map* second, char** only_first, size_t* only_first_size, char** only_second, size_t* only_second_size, char** both_present, size_t* both_present_size);

#endif