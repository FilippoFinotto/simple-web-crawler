#include "queue.h"
#include "hashtable.h"
#include "http.h"
#include "parser.h"

#include <stdio.h>

int main(){

	#define HASH_TABLE_SIZE 1024
	#define MAX_PAGES_TO_CRAWL 10
	int pages_crawled = 0;

	char* seed_url = strdup("https://it.wikipedia.org/wiki/Crawler");
	
	Queue* frontier = create_queue();
	HashTable* visited = create_table(HASH_TABLE_SIZE);
	enqueue(frontier, seed_url);
	free(seed_url);

	while (pages_crawled < MAX_PAGES_TO_CRAWL) {
		char* current_url = dequeue(frontier);
		if(current_url == NULL) break;
		
		if(search(visited, current_url)){
			free(current_url);
			continue;
		}
		insert(visited, current_url);
		
		HttpData* page_data = download_page(current_url);
		if (page_data != NULL) {
			printf("Download riuscito. %zu bytes scaricati.\n", page_data->size);
			Queue *new_links = find_links(page_data->memory, current_url);
			if(!new_links){
				printf("Errore nella ricerca dei nuovi link");
			}
			while(!is_empty(new_links)){
				char *link = dequeue(new_links);
				if(!link){
					printf("Errore nell'estrazione del link dai nuovi link");
				}
				enqueue(frontier, link);
				free(link);
			}
			destroy_queue(new_links);
			free_http_data(page_data);
			pages_crawled++;
		}
		free(current_url);
	}
	print_table(visited);
	destroy_queue(frontier);
	destroy_table(visited);

	return 0;
}

