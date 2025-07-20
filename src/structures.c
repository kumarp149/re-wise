#include "../include/structures.h"

unsigned int hash(const char* key){
    unsigned long long hash = 0;

    while(*key){
        unsigned long long val = (unsigned long long) (*key);
        key++;
        hash = (hash*31) + val;
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

bool hash_map_isempty(hash_map* map){
    if (map == NULL) return true;

    for (int i=0;i<JVC_HASHMAP_SIZE;++i){
        hash_node* node = map->buckets[i];
        while(node){
            return false;
        }
    }
    return true;
}

void hash_map_insert(hash_map* map,const char* key, const char* value){
    unsigned int index = hash(key);

    hash_node* node = map->buckets[index];

    while(node){
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
            temp = NULL;
        }
    }
    __RW_MEMFREE__(map);
}

// char *serialize_hash_map_to_binary(hash_map *map, size_t *binary_size) {
//     // Step 1: Calculate the total size needed for the binary string
//     size_t total_size = 0;
//     for (int i = 0; i < JVC_HASHMAP_SIZE; i++) {
//         hash_node *node = map->buckets[i];
//         while (node) {
//             total_size += sizeof(size_t); // For key length
//             total_size += strlen(node->key) + 1; // For key (including null terminator)
//             total_size += sizeof(size_t); // For value length
//             total_size += strlen(node->value) + 1; // For value (including null terminator)
//             node = node->next;
//         }
//     }

//     // Step 2: Allocate memory for the binary string
//     char *binary_data = (char *)malloc(total_size);
//     if (!binary_data) {
//         //perror("Failed to allocate memory for binary data");
//         exit(EXIT_FAILURE);
//     }

//     // Step 3: Serialize the hash map into the binary string
//     char *ptr = binary_data;
//     for (int i = 0; i < JVC_HASHMAP_SIZE; i++) {
//         hash_node *node = map->buckets[i];
//         while (node) {
//             size_t key_len = strlen(node->key) + 1; // Include null terminator
//             size_t value_len = strlen(node->value) + 1;

//             // Copy key length, key, value length, and value into the binary string
//             memcpy(ptr, &key_len, sizeof(size_t));
//             ptr += sizeof(size_t);
//             memcpy(ptr, node->key, key_len);
//             ptr += key_len;
//             memcpy(ptr, &value_len, sizeof(size_t));
//             ptr += sizeof(size_t);
//             memcpy(ptr, node->value, value_len);
//             ptr += value_len;

//             node = node->next;
//         }
//     }

//     // Step 4: Return the binary string and its size
//     *binary_size = total_size;
//     return binary_data;
// }

char *serialize_hash_map_to_binary(hash_map *map, size_t *binary_size) {
    // Step 1: Calculate total size needed
    size_t total_size = 0;
    for (int i = 0; i < JVC_HASHMAP_SIZE; i++) {
        hash_node *node = map->buckets[i];
        while (node) {
            uint16_t key_len = (uint16_t)strlen(node->key);
            uint8_t val_len = (uint8_t)strlen(node->value);
            total_size += 2 + key_len + 1 + val_len;
            node = node->next;
        }
    }

    // Step 2: Allocate buffer
    uint8_t *buffer = (uint8_t *)malloc(total_size);
    if (!buffer) {
        exit(EXIT_FAILURE);
    }

    // Step 3: Write entries
    uint8_t *ptr = buffer;
    for (int i = 0; i < JVC_HASHMAP_SIZE; i++) {
        hash_node *node = map->buckets[i];
        while (node) {
            uint16_t key_len = (uint16_t)strlen(node->key);
            uint8_t val_len = (uint8_t)strlen(node->value);

            // Write 2-byte key length (little-endian)
            ptr[0] = (uint8_t)(key_len & 0xFF);
            ptr[1] = (uint8_t)((key_len >> 8) & 0xFF);
            ptr += 2;

            // Write key
            memcpy(ptr, node->key, key_len);
            ptr += key_len;

            // Write 1-byte value length
            *ptr++ = val_len;

            // Write value
            memcpy(ptr, node->value, val_len);
            ptr += val_len;

            node = node->next;
        }
    }

    *binary_size = total_size;
    return (char *)buffer;
}

static uint16_t read_uint16_le(const uint8_t *ptr) {
    return (uint16_t)((uint16_t)ptr[0] | ((uint16_t)ptr[1] << 8));
}

hash_map *deserialize_hash_map_from_binary(const char *binary_data, size_t binary_size) {
    const uint8_t *ptr = (const uint8_t *)binary_data;
    const uint8_t *end = ptr + binary_size;

    hash_map *map = create_hash_map();
    if (!map) {
        fprintf(stderr, "Failed to allocate hash map.\n");
        exit(EXIT_FAILURE);
    }

    while (ptr < end) {
        if (end - ptr < 2) {
            fprintf(stderr, "Unexpected end of buffer while reading key length.\n");
            exit(EXIT_FAILURE);
        }

        uint16_t key_len = read_uint16_le(ptr);
        ptr += 2;

        if (end - ptr < key_len) {
            fprintf(stderr, "Unexpected end of buffer while reading key.\n");
            exit(EXIT_FAILURE);
        }

        char *key = (char *)malloc(key_len + 1);
        if (!key) exit(EXIT_FAILURE);
        memcpy(key, ptr, key_len);
        key[key_len] = '\0';
        ptr += key_len;

        if (end - ptr < 1) {
            fprintf(stderr, "Unexpected end of buffer while reading value length.\n");
            free(key);
            exit(EXIT_FAILURE);
        }

        uint8_t val_len = *ptr++;

        if (end - ptr < val_len) {
            fprintf(stderr, "Unexpected end of buffer while reading value.\n");
            free(key);
            exit(EXIT_FAILURE);
        }

        char *value = (char *)malloc(val_len + 1);
        if (!value) {
            free(key);
            exit(EXIT_FAILURE);
        }
        memcpy(value, ptr, val_len);
        value[val_len] = '\0';
        ptr += val_len;

        hash_map_insert(map, key, value);

        // Free temp copies if your insert function duplicates them
        free(key);
        free(value);
    }

    return map;
}

// Function to deserialize a binary string back into a hash map
// hash_map *deserialize_hash_map_from_binary(const char *binary_data, size_t binary_size) {
//     hash_map *map = create_hash_map();
//     if (!map) {
//         //perror("Failed to create hash map");
//         return NULL;
//     }

//     const char *ptr = binary_data;
//     const char *end = binary_data + binary_size;

//     while (ptr < end) {
//         size_t key_len, value_len;

//         // Read key length
//         memcpy(&key_len, ptr, sizeof(size_t));
//         ptr += sizeof(size_t);

//         // Read key
//         char *key = (char *)malloc(key_len);
//         if (!key) {
//             //perror("Failed to allocate memory for key");
//             free_hash_map(map);
//             return NULL;
//         }
//         memcpy(key, ptr, key_len);
//         ptr += key_len;

//         // Read value length
//         memcpy(&value_len, ptr, sizeof(size_t));
//         ptr += sizeof(size_t);

//         // Read value
//         char *value = (char *)malloc(value_len);
//         if (!value) {
//             //perror("Failed to allocate memory for value");
//             free(key);
//             free_hash_map(map);
//             return NULL;
//         }
//         memcpy(value, ptr, value_len);
//         ptr += value_len;

//         // Insert the key-value pair into the hash map
//         hash_map_insert(map, key, value);

//         // Free temporary key and value buffers
//         free(key);
//         free(value);
//     }

//     return map;
// }


void map_get_difference(hash_map* first, hash_map* second, hash_map* only_first, hash_map* only_second, hash_map* both_present){

    for (int i=0;i<JVC_HASHMAP_SIZE;++i){
        hash_node* node = first->buckets[i];

        while(node){
            if (hash_map_get(second,node->key) == NULL){
                hash_map_insert(only_first,node->key,"");
            } else{
                hash_map_insert(both_present,node->key,"");
            }
            node = node->next;
        }
    }

    for (int i=0;i<JVC_HASHMAP_SIZE;++i){
        hash_node* node = second->buckets[i];

        while(node){
            if (hash_map_get(first,node->key) == NULL){
                hash_map_insert(only_second,node->key,"");
            }
            node = node->next;
        }
    }
}

size_t hash_map_getsize(hash_map* map){
    size_t res = 0;
    for (int i=0;i<JVC_HASHMAP_SIZE;++i){
        hash_node* node = map->buckets[i];
        while(node){
            res++;
            node = node->next;
        }
    }

    return res;
}