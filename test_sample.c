#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hashmap.h"

#ifdef COUNTING_WORDS
#include "wordcount.h"
#endif

size_t mystrlen(const char *str) {
    return strlen(str) + 1;
}

void print_element(const char *key, void *data) {
	printf("(\"%s\", %s)\n", key, (char*)data);
}

void *resolve_collision(void *old_value, void *new_value) {
	return new_value;
}

unsigned your_own_hash(const char *key) {
	return (unsigned) strlen(key) + (unsigned) *key;
}

int main() {
	unsigned int key_space = 1024;
	HashMap * hm = create_hashmap(key_space);

	char * string_1 = "TI2725-C";
	char * string_2 = "Embedded";
	char * string_3 = "Software";
	const char * key_1    = "ab";
	const char * key_2    = "cd";
	const char * key_3    = "ad";
	const char * key_4    = "xy";

	insert_data(hm, key_1, string_1, resolve_collision);
	insert_data(hm, key_2, string_2, resolve_collision);
	insert_data(hm, key_3, string_3, resolve_collision);

	remove_data(hm, key_2, NULL);
	remove_data(hm, key_1, NULL);
	remove_data(hm, key_3, NULL);

	// Insert ("ab" -> "TI2725-C").
	insert_data(hm, key_1, string_1, resolve_collision);
	assert(memcmp(get_data(hm, key_1), string_1, mystrlen(string_1)) == 0);

	// Insert ("cd" -> "Embedded").
	insert_data(hm, key_2, string_2, resolve_collision);
	assert(memcmp(get_data(hm, key_2), string_2, mystrlen(string_2)) == 0);

	// Insert ("ad" -> "Software").
	insert_data(hm, key_3, string_3, resolve_collision);
	assert(memcmp(get_data(hm, key_3), string_3, mystrlen(string_3)) == 0);

	// Insert ("ab" -> "Embedded").
	insert_data(hm, key_1, string_2, resolve_collision);
	assert(memcmp(get_data(hm, key_1), string_2, mystrlen(string_2)) == 0);

	// Get data for a not inserted key.
	assert(get_data(hm, key_4) == NULL);

	// Iterate the hash map
	iterate(hm, print_element);

#ifdef NEW_HASH
	set_hash_function(hm, your_own_hash);

	printf("\nHERE WE GO AGAIN!\n\n");

	// Iterate the hash map 
	iterate(hm, print_element);
#endif

	// Delete key "cd".
	remove_data(hm, key_2, NULL);
	assert(get_data(hm, key_2) == NULL);

	// Delete key "ab".
	remove_data(hm, key_1, NULL);
	assert(get_data(hm, key_1) == NULL);

	// Delete key "ad".
	remove_data(hm, key_3, NULL);
	assert(get_data(hm, key_3) == NULL);

	// Delete the hash map.
	delete_hashmap(hm, NULL);

#ifdef COUNTING_WORDS
	// Create a temporary file
	FILE *stream = tmpfile();

	// Write to the stream
	fprintf(stream, "foo bar_, baz!\n");
	fprintf(stream, "foo\t\"bar\".\n");
	fprintf(stream, "foo?\n");

	// Set the position to the start of the stream
	fseek(stream, 0, SEEK_SET);

	// Count the words
	count_words(stream);

	// Close the file
	fclose(stream);
#endif

	hm = create_hashmap(key_space);
	unsigned int space = 32;

	char * allocated_key = (char *)malloc(space);
	char * allocated_data = (char *)malloc(space);

	// set allocated_key to be key_1
	strcpy(allocated_key, key_1);
	// set allocated_data to be string_1
	strcpy(allocated_data, string_1);

	insert_data(hm, allocated_key, allocated_data, resolve_collision);
	assert(strcmp(get_data(hm, key_1), string_1) == 0);

	// change allocated_key and check if key_1 in hashmap is still string_1;
	strcpy(allocated_key, key_2);
	strcpy(allocated_data, string_2);

	// checks if get_data still gives the value of allocated_data after changing keys.
	assert(strcmp(get_data(hm, key_1), allocated_data) == 0); // [ONLINE TESTER] segmentation fault.
	assert(strcmp(get_data(hm, key_1), string_1) != 0);
	
	insert_data(hm, allocated_key, allocated_data, resolve_collision);
	assert(strcmp(get_data(hm, key_2), allocated_data) == 0);

/**
	insert_data(hm, key_3, string_1, NULL);
	assert(memcmp(get_data(hm, key_3), string_1, mystrlen(string_1)) == 0);
	
	// If there is collision the online hashmpa fails if the is no call back function
	insert_data(hm, key_3, string_2, NULL); // [ONLINE TESTER] program failed.
	assert(memcmp(get_data(hm, key_3), string_1, mystrlen(string_1)) == 0);
*/
	delete_hashmap(hm, NULL);



	free(allocated_key);
	free(allocated_data);

	return 0;
}
