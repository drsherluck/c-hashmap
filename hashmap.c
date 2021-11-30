#include "hashmap.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h> 

/*
 * Default hashing function.
 * @param  A character list to be hashed
 * @return A sum of the ascii values of the key.
 */
uint32_t 
hash(const char *key) 
{
    uint32_t sum = 0;
    for (int i = 0; key[i] != '\0'; i++) 
    {
        sum += (uint32_t) key[i];   
    }
    return sum;
}

/*
 * Function used to create a hashmap
 * returns a pointer to the hashmap_t.
 */
hashmap_t* 
create_hashmap(uint32_t key_space) 
{
    hashmap_t *hm = malloc(sizeof(hashmap_t));
    hm->key_space = key_space;
    hm->hash = &hash;
    hm->size = 0;
    hm->elements = calloc(key_space, sizeof(bucket_list));
    return hm;
}

/*
 * creates a bucket.
 * @param key is the bucket key
 * @param data is the data it need to hold.
 */
bucket* 
create_bucket(const char * key, void * data) 
{
    bucket *bk = malloc(sizeof(bucket));
    size_t len = strlen(key) + 1;
    bk->key = malloc(len);
    if (bk->key != NULL)
    {
        bk->key = memcpy(bk->key, key, len);
    }
    bk->data = data;
    bk->prev = NULL;
    bk->next = NULL;
    return bk;
}

bucket_list* 
create_bucketlist() 
{
    bucket_list *list = calloc(1, sizeof(bucket_list));
    return list;
}

/*
 * @return The bucket_list that should contain a bucket with the given key.
 */
bucket_list* 
get_bucket_list(hashmap_t *hm, const char * key) 
{
    uint32_t h = hm->hash(key) % hm->key_space;
    if (hm->elements[h] == NULL) 
    {
        hm->elements[h] = create_bucketlist();
    }
    return hm->elements[h];
}

/*
 * @return the bucket with given key (or NULL if no bucket can be found
 */
bucket* 
get_bucket(bucket_list* blist, const char* key) 
{
    if (blist != NULL) 
    {
        bucket *bk = blist->head;
        while(bk != NULL) 
        {
            if (strcmp(bk->key, key) == 0) 
            {
                return bk;
            }
            bk = bk->next;
        }
    }
    return NULL;
}

/*
 * Removes a bucket from the linked list
 */
void 
delete_bucket(bucket_list *blist, bucket *bk, destroy_data_t destroy_data) 
{
    bucket *prev = bk->prev;
    bucket *next = bk->next;

    // only element
    if (!prev && !next) 
    {
        blist->tail = NULL;
        blist->head = NULL;
    }   
    // tail element
    else if (prev && !next) 
    {
        prev->next = NULL;
        blist->tail = prev;
    } 
    // head element
    else if (next && !prev) 
    {
        next->prev = NULL;
        blist->head = next;
    }
    // middle element
    else if (next && prev) 
    {
        next->prev = prev;
        prev->next = next;
    }

    blist->size--;
    if (destroy_data != NULL) 
    {
        destroy_data(bk->data);
    }
    free(bk->key);
    free(bk);
}

void 
insert_bucket(bucket_list *blist, bucket *bk) 
{
    if (blist->size == 0) 
    {
        // if first entry.
        blist->head = bk;
        blist->tail = bk;
    } else 
    {
        // add to the end of the list
        bk->prev = blist->tail;
        blist->tail->next = bk;
        blist->tail = bk;
    }
}

/*
 * Store the data pointer and copy of the key in the bucket
 * in the hashmap_t *hm, resolve_collision is called in case of
 * collision.
 */
void 
insert_data(hashmap_t *hm, const char *key, void *data, resolve_collision_t resolve_collision) 
{
    if (hm == NULL || key == NULL || hm->key_space <= 0) return;

    bucket_list *blist = get_bucket_list(hm, key);
    bucket *bk = get_bucket(blist, key);

    if (bk == NULL) 
    {
        bk = create_bucket(key, data);
        insert_bucket(blist, bk);
        blist->size++;
        hm->size++;
    } 
    else if (resolve_collision != NULL) 
    {
        bk->data = resolve_collision(bk->data, data);
    }
}

/*
 * Returns a pointer to data stored at key.
 */
void* 
get_data(hashmap_t *hm, const char *key) 
{
    if (hm == NULL || hm->key_space <= 0) return NULL;

    bucket_list *blist = get_bucket_list(hm, key);
    bucket *bk = get_bucket(blist, key);

    if (bk == NULL) 
    {
        return NULL;
    }
    return bk->data;
}


/*
 * Iterator for hashmap_t *hm.
 */
void 
iterate(hashmap_t *hm, void(*callback)(const char *, void *)) 
{
    if (hm == NULL || hm->size <= 0 || callback == NULL) return;

    bucket *bk = NULL;
    for (uint32_t i = 0; i < hm->key_space; i++) 
    {
        if (hm->elements[i] == NULL) 
        {
            continue;
        } 
        bk = hm->elements[i]->head;
        while (bk != NULL) 
        {
            callback(bk->key, bk->data); 
            bk = bk->next;
        }
    }
}

/*
 * Function for removing data at key position.
 */
void 
remove_data(hashmap_t *hm, const char *key, destroy_data_t destroy_data)
{
    if (hm == NULL || hm->key_space <= 0) return;

    bucket_list *blist = get_bucket_list(hm, key);
    bucket *bk = get_bucket(blist, key);

    if (bk != NULL) 
    {
        delete_bucket(blist, bk, destroy_data);
        hm->size--;
    }
}

/*
 * Deletes the entire hashmap
 */
void 
delete_hashmap(hashmap_t *hm, destroy_data_t destroy_data) 
{
    if (hm == NULL) return;

    bucket *bk = NULL;
    bucket *temp = NULL;
    bucket_list *blist = NULL;
    for (uint32_t i = 0; i < hm->key_space; i++) 
    {
        blist = hm->elements[i];
        if (blist == NULL) 
        {
            continue;
        }
        bk = blist->head;
        while (bk != NULL) 
        {
            temp = bk->next;
            delete_bucket(blist, bk, destroy_data);
            bk = temp;
        }
        free(blist);
    }

    free(hm->elements);
    free(hm);
}

/*
 * Rehashes the given hashmap.
 */
void 
rehash(hashmap_t *hm) 
{
    if (hm == NULL || hm->size <= 0) return;

    bucket_list *(*temp) = calloc(hm->key_space, sizeof(bucket_list));
    bucket_list *(*old)  = hm->elements;
    hm->elements = temp;
    hm->size = 0;

    bucket *bk = NULL;
    bucket *t_bk = NULL;
    for (uint32_t i = 0; i < hm->key_space; i++) 
    {
        if (old[i] != NULL) 
        {
            bk = old[i]->head;
            while (bk != NULL) 
            {
                insert_data(hm, bk->key, bk->data, NULL);
                t_bk = bk->next;
                free(bk->key);
                free(bk);
                bk = t_bk;
            }
            free(old[i]);
        }
    }
    free(old);
}

/*
 * Sets a new hashing function for the hashmap_t *hm.
 */
void 
set_hash_function(hashmap_t *hm, hash_function_t hash_function)
{
    if (hm == NULL || hash_function == NULL) return;
    hm->hash = hash_function;
    rehash(hm); 
}

