#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct HashNode {
	char *url;
	struct HashNode *next;
}HashNode;

typedef struct HashTable {
	int size;
	struct HashNode **table;
}HashTable;

HashTable* create_table (int size);
unsigned long hash(const char *url);
void insert(HashTable *ht, const char *url);
int search(HashTable *ht, const char *url);
void destroy_table(HashTable *ht);
void print_table(HashTable *ht);

#endif





















