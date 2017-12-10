#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>

/**
	Represents a bucket in the a HashMap.
*/
typedef struct bucket {
	void * data;
	char * key;
	struct bucket * next;
	struct bucket * prev;
} bucket;

/**
	Represents a linked-list of buckets. 
*/
typedef struct bucket_list {
	int size;
	bucket * head;
	bucket * tail;
} bucket_list;

/**
	Represent a HashMap.
*/
typedef struct HashMap {
	size_t key_space;
	int size;
	unsigned int (*hash)(const char *);
	bucket_list *(*elements);
} HashMap;


unsigned int hash(const char * key);
HashMap * create_hashmap(size_t key_space);
void insert_data(HashMap *hm, const char *key, void *data, void *(*resolve_collision)(void*, void*));
void * get_data(HashMap *hm, const char *key);
void iterate(HashMap *hm, void(*callback)(const char *, void *));
void remove_data(HashMap *hm, const char *key, void(*destroy_data)(void *));
void delete_hashmap(HashMap *hm, void(*destroy_data)(void *));

#endif

#ifndef NEW_HASH
#define NEW_HASH

void set_hash_function(HashMap *hm, unsigned int (*hash_function)(const char *));

#endif


