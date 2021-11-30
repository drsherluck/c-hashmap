#include "wordcount.h"
#include "hashmap.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *
copy_str(const char* str)
{
    size_t len = strlen(str) + 1;
    char* res = malloc(len);
    if (res != NULL)
    {
        res = memcpy(res, str, len);
    }
    return res;
}

char*
read_word(FILE * stream)
{
    char buf[100];
    int ch;
    // skip non alphabetic chars
    while(EOF != (ch=fgetc(stream)) && !isalpha(ch)) {}; 
    if (ch == EOF) return NULL;

    size_t i;
    i = 0;
    do {
        buf[i++] = ch;
    } while(EOF != (ch = fgetc(stream)) && isalpha(ch) && i < sizeof(buf) - 1);
    buf[i] = '\0';
    return copy_str(buf);
}

void 
print_wordcount(const char* key, void* value)
{
    int val = *((int*)value);
    printf("(%s, %d)\n", key, val);
}

void*
increase_count(void* curr, void* next)
{
    int a = *((int*)next);
    int* b = (int*)curr;
    *b += a;
    free(next);
    return b;
}

/*Counts how many times a word is in a file.*/
void 
count_words(FILE * stream) 
{
    hashmap_t* hm = create_hashmap(1024);
    int count = 1;
    char* word;
    while (word = read_word(stream)) 
    {
        void* data = malloc(sizeof(int));
        insert_data(hm, word, memcpy(data, &count, sizeof(int)), &increase_count);
        free(word);
    } 
    iterate(hm, &print_wordcount);
    delete_hashmap(hm, &free);
}
