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
	hm->elements = (bucket *)calloc(key_space, sizeof(bucket_list));
	return hm;
}

/**
	creates a bucket.
	@param key is the bucket key
	@param data is the data it need to hold.
*/
bucket * create_bucket(const char * key, void * data) {
	bucket *bk = (bucket *)malloc( sizeof (bucket) );
	bk->key = (char *) strndup(key, strlen(key) + 1);
	bk->data = data;
	bk->prev = NULL;
	bk->next = NULL;
	return bk;
}

/**
	Returns the bucket_list that should contain bucket with key.
*/
bucket_list * get_bucket_list(HashMap *hm, const char * key) {
	unsigned int h = hm->hash(key) % hm->key_space;
	return hm->elements[h];
}

/**
	Returns a bucket with the given key.
	return NULL if that bucket is not available.
*/
bucket * get_bucket(bucket_list *blist, const char * key) {
	bucket *bk = blist->head;
	while(bk != NULL) {
		if (strcmp(bk->key, key) == 0) {
			return bk;
		}
		bk = bk->next;
	}
	return NULL;
}

/**
	Deletes a bucket.
*/
void delete_bucket(bucket *bk) {
	bucket *prev = bk->prev;
	bucket *next = bk->next;

	if (next == NULL) {
		prev->next = next;
	} else if (prev == NULL) {
		next->prev = prev;
	} else if (prev != NULL && next != NULL) {
		prev->next = next;
		next->prev = prev;
	}

	free(bk->key);
	free(bk);
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

	bucket_list *blist = get_bucket_list(hm, key);
	bucket *bk = get_bucket(blist, key);

	if (bk == NULL) {
		bk = create_bucket(key, data);
		blist->size++;
	} else if (resolve_collision != NULL) {
		bk->data = resolve_collision(bk->data, data);
	}

}

/**
	Returns a pointer to data stored at key.
*/
void * get_data(HashMap *hm, const char *key) {
	if (hm == NULL || hm->key_space <= 0) {
		return NULL;
	}

	bucket_list *blist = get_bucket_list(hm, key);
	bucket *bk = get_bucket(blist, key);

	if (bk == NULL) {
		return NULL;
	}
	return bk->data;
}


/**
	Iterator for HashMap *hm.
*/
void iterate(HashMap *hm, void(*callback)(const char *, void *)) {
	if (hm == NULL || hm->size == 0 || callback == NULL) {
		return;
	}
	bucket *bk = NULL;
	for (unsigned int i = 0; i < hm->key_space; i++) {
		bk = hm->elements[i].head;
		while (bk != NULL) {
			callback(bk->key, bk->data);	
			bk = bk->next;
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

	bucket_list *blist = get_bucket_list(hm, key);
	bucket *bk = get_bucket(blist, key);
	if (bk == NULL) {
		return;
	}
	if (destroy_data != NULL) {
		destroy_data(bk->data);
	}
	delete_bucket(bk);
	blist->size--;
}

/**
	Deletes the entire HashMap *hm.
*/
void delete_hashmap(HashMap *hm, void(*destroy_data)(void *)) {
	if (hm == NULL) {
		return;
	}

	bucket_list *blist = get_bucket_list(hm, key);
	bucket *bk = get_bucket(blist, key);

	bucket *temp = NULL;
	for (unsigned int i = 0; i < hm->key_space; i++) {
		bk = hm->elements[i].head;
		while (bk != NULL) {
			if (destroy_data != NULL) {
				destroy_data(bk->data);
			}
			temp = bk->next;
			delete_bucket(bk);
			bk = temp;
		}
	}

	free(hm->elements);
	free(hm);
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
