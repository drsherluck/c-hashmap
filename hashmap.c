#include "hashmap.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h> // need to remove this and all asserts afterwards.


/**
	Default hashing function.
	returns sum of ascii values of letters.
*/
unsigned int hash(const char *key) {
	unsigned int sum = 0;
	for (int i = 0; key[i] != '\0'; i++) {
		sum += (unsigned int) key[i];	
	}
	return sum;
}

/**
	Function used to create a hashmap
	returns a pointer to the HashMap.
*/
HashMap * create_hashmap(size_t key_space) {
	HashMap *hm = (HashMap *)malloc( sizeof(HashMap) );
	hm->key_space = key_space;
	hm->hash = &hash;
	hm->size = 0;
	// initialize key_space amount of elements and sets them to 0 or NULL.
	hm->elements = (bucket *)calloc(key_space, sizeof(bucket));
	return hm;
}

/**
	Store the data pointer and copy of the key in the bucket
	in the HashMap *hm, resolve_collision is called in case of
	collision.
*/
void insert_data(HashMap *hm, const char *key, void *data, void *(*resolve_collision)(void*, void*)) {
	if (hm == NULL || key == NULL || hm->key_space <= 0) {
		return;
	}

	unsigned int h = hm->hash(key) % hm->key_space;

	if ( hm->elements[h].has_data ) {
		if (resolve_collision == NULL)
			return;
		free(hm->elements[h].key);
		hm->elements[h].data = resolve_collision(hm->elements[h].data, data);
	} else {
		hm->elements[h].has_data = 1;
		hm->elements[h].data = data;
		hm->size++;
	}
	hm->elements[h].key = (char *) strndup(key, strlen(key) + 1);
} 

/**
	Determines what data is stored in the hashmap in case of
	a key collision by returning the void pointer to the data
	that is to be stored.
*/
void * ResolveCollisionCallback(void *old_data, void* new_data) {
	return new_data;
}

/**
	Returns a pointer to data stored at key.
*/
void * get_data(HashMap *hm, const char *key) {
	if (hm == NULL || hm->key_space <= 0) {
		return NULL;
	}
	unsigned int h = hm->hash(key) % hm->key_space;
	return hm->elements[h].data;
}


/**
	Iterator for HashMap *hm.
*/
void iterate(HashMap *hm, void(*callback)(const char *, void *)) {
	if (hm == NULL || hm->size == 0 || callback == NULL) {
		return;
	}
	for (unsigned int i = 0; i < hm->key_space; i++) {
		if (hm->elements[i].has_data) {
			assert(i == get_index(hm, hm->elements[i].key));
			callback(hm->elements[i].key, hm->elements[i].data);		
		}
	}
}

/**
	Function for removing data at key position.
*/
void remove_data(HashMap *hm, const char *key, void(*destroy_data)(void *)) {
	if (hm == NULL || hm->key_space <= 0) {
		return;
	}
	unsigned int h = hm->hash(key) % hm->key_space;
	if (!hm->elements[h].has_data) {
		return;
	}
	free(hm->elements[h].key);
	if (destroy_data == NULL) {
		hm->elements[h].data = NULL;
	} else {
		destroy_data(hm->elements[h].data);
	}
	hm->elements[h].has_data = 0;
	hm->size--; // decrease size;
}

/**
	Deletes the entire HashMap *hm.
*/
void delete_hashmap(HashMap *hm, void(*destroy_data)(void *)) {
	if (hm == NULL) {
		return;
	}

	for (unsigned int i = 0; i < hm->key_space; i++) {
		if (destroy_data != NULL) {
			destroy_data(hm->elements[i].data);
		}
		if (hm->elements[i].has_data) {
			free(hm->elements[i].key);
			hm->elements[i].data = NULL;
			hm->size--;
			hm->elements[i].has_data = 0;
		}
	}

	free(hm->elements);
	free(hm);
}
 
/**
	Callback function used in remove_data when deleting data 
	from the HashMap.
*/
void DestroyDataCallback(void *data) {
	if (data != NULL) {
		free(data);
	}
}

/**
	Returns the index of a given key using the hashing function
	of HashMap hm.
*/
unsigned int get_index(HashMap *hm, const char * key) {
	if (hm == NULL) {
		return 0;
	}
	return hm->hash(key) % hm->key_space;
}

/** BONUS PART **/

/**
	Rehashes the given hashmap.
*/
void rehash(HashMap *hm) {
	if (hm == NULL || hm->size <= 0) {
		return;
	}

	bucket *temp = (bucket *)calloc(hm->key_space, sizeof(bucket));
	bucket *old_data = hm->elements;
	hm->elements = temp;	

	for (unsigned int i = 0; i < hm->key_space; i++) {
		insert_data(hm, old_data[i].key, old_data[i].data, NULL);
		free(old_data[i].key);
	}

	free(old_data);
}

/**
	Sets a new hashing function for the HashMap *hm.
*/
void set_hash_function(HashMap *hm, unsigned int (*hash_function)(const char *)) {
	if (hm == NULL || hash_function == NULL) {
		return;
	}
	hm->hash = hash_function;
	rehash(hm);
}

/**
	Counts how many times a word is in a file.
*/
void count_words(FILE * stream) {

}
