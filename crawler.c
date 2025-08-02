#include "queue.h"
#include "hashtable.h"
#include "http.h"

#include <stdio.h>





int main(){

	#define HASH_TABLE_SIZE 1024
	const char* seed_url = "https://it.wikipedia.org/wiki/Crawler";
	
	Queue* frontier = create_queue();
	HashTable* visited = create_table(HASH_TABLE_SIZE);
	enqueue(frontier, seed_url);
	if (!is_empty(frontier)) {
		char* current_url = dequeue(frontier);
		if (!search(visited, current_url)){
			printf("Crawling: %s\n", current_url);
			insert(visited, current_url);
			HttpData* page_data = download_page(current_url);
			if (page_data != NULL) {
				printf("Download riuscito. %zu bytes scaricati.\n", page_data->size);
				free_http_data(page_data);
			} else {
				printf("Download fallito per: %s\n", current_url);
			}
		free(current_url);

		}
	}
	destroy_queue(frontier);

	return 0;
}
