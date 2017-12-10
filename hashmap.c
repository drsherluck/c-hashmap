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
	hm->elements = calloc(key_space, sizeof(bucket_list));
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
*/
bucket_list * create_bucketlist() {
	bucket_list *blist = (bucket_list *)malloc( sizeof(bucket_list) );
	blist->head = NULL;
	blist->tail = NULL;
	blist->size = 0;
	return blist;
}

/**
	Returns the bucket_list that should contain bucket with key.
*/
bucket_list * get_bucket_list(HashMap *hm, const char * key) {
	unsigned int h = hm->hash(key) % hm->key_space;
	if (hm->elements[h] == NULL) {
		hm->elements[h] = create_bucketlist();
	}
	return hm->elements[h];
}

/**
	Returns a bucket with the given key.
	return NULL if that bucket is not available.
*/
bucket * get_bucket(bucket_list *blist, const char * key) {
	if (blist == NULL) {
		return NULL;
	}
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
void delete_bucket(bucket_list *blist, bucket *bk) {
	bucket *prev = bk->prev;
	bucket *next = bk->next;

	if (next == NULL && prev != NULL) {
		prev->next = NULL;
		blist->tail = prev;
	} 
	if (prev == NULL && next != NULL) {
		next->prev = NULL;
		blist->head = next;
	} 
	if (prev != NULL && next != NULL) {
		prev->next = next;
		next->prev = prev;
	}

	free(bk->key);
	free(bk);
}

void insert_bucket(bucket_list * blist, bucket * bk) {
	if (blist->head == NULL) {
		blist->head = bk;
		blist->tail = bk;
	} else {
		bucket *temp = blist->tail;
		temp->next = bk;
		bk->prev = temp;
		bk->next = NULL;
	}
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
		insert_bucket(blist, bk);
		blist->size++;
		hm->size++;
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
	if (hm == NULL || hm->size <= 0 || callback == NULL) {
		return;
	}
	bucket *bk = NULL;
	for (unsigned int i = 0; i < hm->key_space; i++) {
		if (hm->elements[i] == NULL) {
			continue;
		} 
		bk = hm->elements[i]->head;
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
	delete_bucket(blist, bk);
	blist->size--;
	hm->size--;
}

/**
	Deletes the entire HashMap *hm.
*/
void delete_hashmap(HashMap *hm, void(*destroy_data)(void *)) {
	if (hm == NULL) {
		return;
	}

	bucket *bk = NULL;
	bucket *temp = NULL;
	bucket_list *blist = NULL;
	for (unsigned int i = 0; i < hm->key_space; i++) {
		blist = hm->elements[i];
		if (blist == NULL) {
			continue;
		}
		bk = blist->head;
		while (bk != NULL) {
			if (destroy_data != NULL) {
				destroy_data(bk->data);
			}
			temp = bk->next;
			delete_bucket(blist, bk);
			bk = temp;
		}
		free(blist);
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

	bucket_list *(*temp) = calloc(hm->key_space, sizeof(bucket_list) );
	bucket_list *(*old) = hm->elements;
	hm->elements = temp;
	hm->size = 0;

	bucket *bk = NULL;
	for (unsigned int i = 0; i < hm->key_space; i++) {
		if (old[i] == NULL) {
			continue;
		}
		bk = old[i]->head;
		while (bk != NULL) {
			insert_data(hm, bk->key, bk->data, NULL);
			free(bk->key);
			bk = bk->next;
		}
	}
	free(old);
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
