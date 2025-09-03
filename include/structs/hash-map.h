#ifndef __STRUCT_HASHMAP__

#define __STRUCT_HASHMAP__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../error.h"
#include "../global.h"
#include <string.h>
#include <pthread.h>

#define JVC_HASHMAP_SIZE 1024

typedef struct hash_node {
    char *key;
    char *value;
    struct hash_node *next;
} hash_node;

typedef struct hash_map{
    hash_node* buckets[JVC_HASHMAP_SIZE];
    pthread_mutex_t lock[JVC_HASHMAP_SIZE];
} hash_map;

unsigned int hash(const char* key);

hash_map* create_hash_map();

bool hash_map_isempty(hash_map* map);

void hash_map_insert(hash_map* map,const char* key, const char* value);

char* hash_map_get(hash_map* map,const char* key);

void free_hash_map(hash_map* map);

char* serialize_hash_map_to_binary(hash_map *map, size_t *binary_size);

hash_map* deserialize_hash_map_from_binary(const char *binary_data, size_t binary_size);

void map_get_difference(hash_map* first, hash_map* second, hash_map* only_first, hash_map* only_second, hash_map* both_present);

size_t hash_map_getsize(hash_map* map);

#endif