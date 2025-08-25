#include "hashtable.h"


/* This function allocates the memory for the table and all his nodes. */

HashTable* create_table(int size) {
	HashTable *new_table = (HashTable *)malloc(sizeof(HashTable));
	new_table->size = size;
	new_table->table = malloc(size*sizeof(HashNode *));
	for(int i = 0; i < new_table->size; i++) {
		new_table->table[i] = NULL;
	}
	return new_table;
}

/* This is the C implememntaton of the djb2 hashing algorithm */

unsigned long hash_function(const char *url) {
	unsigned long hash = 5381;
	int c;
	while((c = *url++)) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}

/* This function takes the pointer to the table and the url, calculates the index of the url and inserts the index 
 * in his bucket if its not already there. */

void insert(HashTable *ht, const char *url) {
	if(ht == NULL || url ==NULL) return;
	
	unsigned int index = hash_function(url) % ht->size; // The index is the hash value associated with the url modulo the table size

	if(search(ht, url) == 0){
		HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
		if(new_node == NULL) return;

		new_node->url = strdup(url);
		if(new_node->url == NULL){
			free(new_node);
			return;
		}

		new_node->next = ht->table[index];
		ht->table[index] = new_node;
	}
}

/* This function search is a url is already in the table, returns 1 if it is 0 otherwise.*/

int search(HashTable *ht, const char *url) {
	int index = hash_function(url) % ht->size;

        HashNode *current = ht->table[index];
        while(current != NULL) {
                if(strcmp(current->url, url)==0) return 1;
                current = current -> next;
        }
	return 0;
}

/* This function deallocates the memory of every node and url and then of the table. */

void destroy_table(HashTable *ht) {
	if(ht == NULL) return;

	for(int i=0; i<ht->size; i++){
		HashNode *current = ht->table[i];
		while(current != NULL){
			HashNode *next_node = current->next;
			free(current->url);
			free(current);
			current = next_node;
		}	
	}
	free(ht->table);
	free(ht);
}

void print_table(HashTable *ht) {
    printf("--- Inizio Contenuto HashTable ---\n\n");

    if (ht == NULL) {
        printf("La HashTable è NULL.\n");
        printf("--- Fine Contenuto HashTable ---\n\n");
        return;
    }

    int total_entries = 0;

    for (int i = 0; i < ht->size; i++) {
        HashNode *current = ht->table[i];

        if (current != NULL) {
            printf("Bucket[%4d]: ", i);

            while (current != NULL) {
                printf("\"%s\" -> ", current->url);
                total_entries++;
                current = current->next; 
            }
            printf("NULL\n");
        }
    }
    
    printf("\nTotale URL nella HashTable: %d\n", total_entries);
    printf("--- Fine Contenuto HashTable ---\n\n");
}





















