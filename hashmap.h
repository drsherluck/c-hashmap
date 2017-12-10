#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>

/**
	Represents a bucket in the a HashMap.
*/
typedef struct bucket {
	// for when user decides to uses NULL as data when 
	// inserting and we need a way to check if a index in
	// the hasmap has been used.
	int has_data;
	void * data;
	char * key;
} bucket;

/**
	Represent a HashMap.
*/
typedef struct HashMap {
	size_t key_space;
	int size;
	unsigned int (*hash)(const char *);
	bucket * elements;
} HashMap;


unsigned int hash(const char * key);
HashMap * create_hashmap(size_t key_space);
void insert_data(HashMap *hm, const char *key, void *data, void *(*resolve_collision)(void*, void*));
void * ResolveCollisionCallback(void *old_data, void* new_data);
void * get_data(HashMap *hm, const char *key);
void iterate(HashMap *hm, void(*callback)(const char *, void *));
void remove_data(HashMap *hm, const char *key, void(*destroy_data)(void *));
void delete_hashmap(HashMap *hm, void(*destroy_data)(void *));
void DestroyDataCallback(void *data);
unsigned int get_index(HashMap *hm, const char * key);

#endif

#ifndef NEW_HASH
#define NEW_HASH

void set_hash_function(HashMap *hm, unsigned int (*hash_function)(const char *));

#endif


