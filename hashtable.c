#include "hashtable.h"

HashTable* create_table(int size) {
	HashTable *new_table = (HashTable *)malloc(sizeof(HashTable));
	new_table->size = size;
	new_table->table = malloc(size*sizeof(HashNode *));
	for(int i = 0; i < new_table->size; i++) {
		new_table->table[i] = NULL;
	}
	return new_table;
}

unsigned long hash_function(const char *url) {
	unsigned long hash = 5381;
	int c;
	while(c = *url++) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}

void insert(HashTable *ht, const char *url) {

	int index = hash_function(url) % ht->size;

	HashNode *current = ht->table[index];
	while(current != NULL) {
		if(strcmp(current->url, url)==0) return
		current = current -> next;
	}

	HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
	new_node->url = malloc(strlen(url)+1);
        strcpy(new_node->url, url);
	new_node->next = ht->table[index];
	ht->table[index] = new_node;
}

int search(HashTable *ht, const char *url) {
	int index = hash_function(url) % ht->size;

        HashNode *current = ht->table[index];
        while(current != NULL) {
                if(strcmp(current->url, url)==0) return 1;
                current = current -> next;
        }
	return 0;
}


void destroy_table(HashTable *ht) {
	for(int i=0; i<ht->size; i++){
		HashNode *current = ht->table[i];
		while(current != NULL){
			HashNode *next_node = current->next;
			free(current->url);
			free(current);
			current = next_node;
		}	
		free(ht->table);
		free(ht);
	}
}
























