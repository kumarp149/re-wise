#include "../include/structures.h"

unsigned int hash(const char* key){
    unsigned long long hash = 0;

    while(*key){
        hash = (hash*31) + *key++;
    }

    return hash % JVC_HASHMAP_SIZE;
}

hash_map* create_hash_map(){
    hash_map* map = (hash_map *) malloc(sizeof(hash_map));
    if (!map){
        //panic!
    }
    memset(map->buckets,0,sizeof(map->buckets));
    return map;
}

void hash_map_insert(hash_map* map,const char* key, const char* value){
    unsigned int index = hash(key);

    //printf("hash_map_insert: key: %d, value: %s\n",index,value);

    hash_node* node = map->buckets[index];

    while(node){
        //printf("hash_map_insert iterating: key: %s\n",node->key);
        if (strcmp(node->key,key) == 0){
            free(node->value);
            node->value = strdup(value);
            return;
        }
        node = node->next;
    }

    node = (hash_node *) malloc(sizeof(hash_node));
    if (!node){
        //panic
    }
    node->key = strdup(key);
    node->value = strdup(value);
    node->next = map->buckets[index];
    map->buckets[index] = node;
}

char* hash_map_get(hash_map* map,const char* key){
    unsigned int index = hash(key);

    hash_node* node = map->buckets[index];

    while(node){
        if (strcmp(node->key,key) == 0){
            return node->value;
        }
        node = node->next;
    }
    return NULL;
}

void free_hash_map(hash_map* map){
    for (int i=0;i<JVC_HASHMAP_SIZE;++i){
        hash_node* node = map->buckets[i];

        while(node){
            hash_node* temp = node;
            node = node->next;
            free(temp->key);
            free(temp->value);
            free(temp);
        }
    }
    free(map);
}

char *serialize_hash_map_to_binary(hash_map *map, size_t *binary_size) {
    // Step 1: Calculate the total size needed for the binary string
    size_t total_size = 0;
    for (int i = 0; i < JVC_HASHMAP_SIZE; i++) {
        hash_node *node = map->buckets[i];
        while (node) {
            total_size += sizeof(size_t); // For key length
            total_size += strlen(node->key) + 1; // For key (including null terminator)
            total_size += sizeof(size_t); // For value length
            total_size += strlen(node->value) + 1; // For value (including null terminator)
            node = node->next;
        }
    }

    // Step 2: Allocate memory for the binary string
    char *binary_data = (char *)malloc(total_size);
    if (!binary_data) {
        //perror("Failed to allocate memory for binary data");
        exit(EXIT_FAILURE);
    }

    // Step 3: Serialize the hash map into the binary string
    char *ptr = binary_data;
    for (int i = 0; i < JVC_HASHMAP_SIZE; i++) {
        hash_node *node = map->buckets[i];
        while (node) {
            size_t key_len = strlen(node->key) + 1; // Include null terminator
            size_t value_len = strlen(node->value) + 1;

            // Copy key length, key, value length, and value into the binary string
            memcpy(ptr, &key_len, sizeof(size_t));
            ptr += sizeof(size_t);
            memcpy(ptr, node->key, key_len);
            ptr += key_len;
            memcpy(ptr, &value_len, sizeof(size_t));
            ptr += sizeof(size_t);
            memcpy(ptr, node->value, value_len);
            ptr += value_len;

            node = node->next;
        }
    }

    // Step 4: Return the binary string and its size
    //printf("exiting");
    *binary_size = total_size;
    return binary_data;
}

// Function to deserialize a binary string back into a hash map
hash_map *deserialize_hash_map_from_binary(const char *binary_data, size_t binary_size) {
    hash_map *map = create_hash_map();
    if (!map) {
        //perror("Failed to create hash map");
        return NULL;
    }

    const char *ptr = binary_data;
    const char *end = binary_data + binary_size;

    while (ptr < end) {
        size_t key_len, value_len;

        // Read key length
        memcpy(&key_len, ptr, sizeof(size_t));
        ptr += sizeof(size_t);

        // Read key
        char *key = (char *)malloc(key_len);
        if (!key) {
            //perror("Failed to allocate memory for key");
            free_hash_map(map);
            return NULL;
        }
        memcpy(key, ptr, key_len);
        ptr += key_len;

        // Read value length
        memcpy(&value_len, ptr, sizeof(size_t));
        ptr += sizeof(size_t);

        // Read value
        char *value = (char *)malloc(value_len);
        if (!value) {
            //perror("Failed to allocate memory for value");
            free(key);
            free_hash_map(map);
            return NULL;
        }
        memcpy(value, ptr, value_len);
        ptr += value_len;

        // Insert the key-value pair into the hash map
        hash_map_insert(map, key, value);

        // Free temporary key and value buffers
        free(key);
        free(value);
    }

    return map;
}