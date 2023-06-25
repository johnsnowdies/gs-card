#include <stdio.h>
#include <alloc.h>
#include <limits.h>
#include "structs.h"
#include "hash.h"
int counter = 0;

int getCounter(){
	return counter;
}

TABLE * CreateHashtable(int size)
{
	int i;
	TABLE *hashtable;

	if ( (hashtable = (TABLE *) malloc( sizeof(TABLE))) == NULL){
		printf("HASH: Cant allocate memory!\n");
		exit(1);
	}

	if ( (hashtable->data = (ENTRY **) malloc(size * sizeof(ENTRY *))) == NULL){
		printf("HASH: Cant allocate memory!\n");
		exit(1);
	}

	for (i = 0; i < size; i++)
	{
		hashtable->data[i] = NULL;
	}

	hashtable->size = size;
	return hashtable;
}

int GetHash(TABLE *hashtable, char *key)
{
	unsigned long int hashval=0;
	int i = 0;

	while (hashval < ULONG_MAX && i != strlen(key))
	{
		hashval = hashval << 8;
		hashval += key[i];
		i++;
	}

	return hashval % hashtable->size;
}

ENTRY *NewPair(char *key, int value)
{
	ENTRY *newpair;


	if( (newpair = malloc(sizeof(ENTRY)) ) == NULL)
	{
		return NULL;
	}

	if ( (newpair->key = strdup(key)) == NULL){
		return NULL;
	}

	counter++;

	newpair->value = value;

	newpair->next = NULL;
	return newpair;

}

void Set(TABLE *hashtable, char *key, int value)
{
	int bin = 0;
	ENTRY *newpair = NULL;
	ENTRY *next = NULL;
	ENTRY *last = NULL;

	bin = GetHash(hashtable, key);

	next = hashtable->data[bin];

	while (next != NULL && next->key != NULL && strcmp(key,next->key) > 0){
		last = next;
		next = next->next;
	}

	if ( next != NULL && next-> key != NULL && strcmp(key, next->key) == 0)
	{
		next->value = value;
	}
	else
	{
		newpair = NewPair(key, value);

		if (next == hashtable->data[bin] ){
			newpair->next = next;
			hashtable->data[bin] = newpair;
		} else if (next == NULL) {
			last->next = newpair;
		} else {
			newpair->next = next;
			last->next = newpair;
		}


	}


}

int Get(TABLE *hashtable, char *key)
{
	int bin = 0;
	ENTRY *pair;
	bin = GetHash(hashtable, key);

	pair = hashtable->data[bin];
	while(pair != NULL && pair->key !=NULL && strcmp(key, pair->key) > 0) {
		pair = pair->next;
	}

	if (pair == NULL || pair->key == NULL || strcmp(key,pair->key) != 0){
		return NULL;
	}
	else
	{
		return pair->value;
	}
}
