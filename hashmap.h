#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#include <stdint.h>

#define NEW_HASH

// function pointer definitions
typedef uint32_t (*hash_function_t)(const char*);
typedef void (*destroy_data_t)(void *);
typedef void (*iterate_callback_t)(const char*, void*);
typedef void* (*resolve_collision_t)(void*, void*);

typedef struct bucket 
{
	void          *data;
	char          *key;
	struct bucket *next;
	struct bucket *prev;
} bucket;

// linked list of buckets
typedef struct bucket_list 
{
	size_t   size;
	bucket   *head;
	bucket   *tail;
} bucket_list;

typedef struct hashmap_t 
{
	uint32_t         key_space;
	int32_t          size;
	hash_function_t  hash;
	bucket_list      *(*elements);
} hashmap_t;


uint32_t   hash(const char* key);
hashmap_t* create_hashmap(uint32_t key_space);
void       insert_data(hashmap_t *hm, const char *key, void *data, resolve_collision_t resolve_collision);
void       remove_data(hashmap_t *hm, const char *key, destroy_data_t destroy_data);
void*      get_data(hashmap_t *hm, const char *key);
void       iterate(hashmap_t *hm, iterate_callback_t callback);
void       delete_hashmap(hashmap_t *hm, destroy_data_t destroy_data);

#ifdef NEW_HASH
void set_hash_function(hashmap_t *hm, hash_function_t hash_function);
#endif
#endif



