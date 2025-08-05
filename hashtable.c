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
	while((c = *url++)) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}

void insert(HashTable *ht, const char *url) {
	if(ht == NULL || url ==NULL) return;
	
	unsigned int index = hash_function(url) % ht->size;

	HashNode *current = ht->table[index];
	while(current != NULL) {
		if(strcmp(current->url, url)==0) return;
		current = current -> next;
	}

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

    // Ciclo FOR esterno: scorre l'array dei bucket
    for (int i = 0; i < ht->size; i++) {
        // Prende il puntatore al primo nodo del bucket corrente
        HashNode *current = ht->table[i];

        // Stampa il contenuto del bucket solo se non è vuoto
        if (current != NULL) {
            printf("Bucket[%4d]: ", i);

            // Ciclo WHILE interno: scorre la lista concatenata nel bucket
            while (current != NULL) {
                printf("\"%s\" -> ", current->url);
                total_entries++;
                current = current->next; // Passa al prossimo nodo nella catena
            }
            printf("NULL\n");
        }
    }
    
    printf("\nTotale URL nella HashTable: %d\n", total_entries);
    printf("--- Fine Contenuto HashTable ---\n\n");
}





















