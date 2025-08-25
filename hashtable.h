#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The HashTable is implemented using an array of pointers to pointers to the head of a linked
 * list, and since I'm using separate chaining to solve collisions the nodes of the list are elements of 
 * the linked lists containing the url
 *
 * table[index]: ---> [node.url | node.next] ----> [node.url | NULL] 
 * table[index2]: ---> [node.url | node.next] ----> NULL
 *
 * .*/

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





















