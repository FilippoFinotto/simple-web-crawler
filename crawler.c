#include "queue.h"
#include "hashtable.h"
#include "http.h"
#include "parser.h"

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define HASH_TABLE_SIZE 1000 
#define MAX_PAGES_TO_CRAWL 100
#define THREAD_NUM 32 


typedef struct{
	Queue *frontier;
	HashTable *visited;

	int *pages_crawled;
	int max_pages;

	pthread_mutex_t *frontier_lock;
	pthread_mutex_t *visited_lock;
	pthread_mutex_t *pages_crawled_lock;

} WorkerArgs;


void* routine(void *thread_args){
	WorkerArgs *args = (WorkerArgs*)thread_args;
	while(1){
		pthread_mutex_lock(args->pages_crawled_lock);
		if(*(args->pages_crawled) >= args->max_pages){
			pthread_mutex_unlock(args->pages_crawled_lock);
			break;
		}	
		pthread_mutex_unlock(args->pages_crawled_lock);
		char *current_url;
		pthread_mutex_lock(args->frontier_lock);
		current_url = dequeue(args->frontier);
		pthread_mutex_unlock(args->frontier_lock);
		if(current_url == NULL){
			sleep(1);
			continue;
		}
		int should_crawl = 0; 
		pthread_mutex_lock(args->visited_lock);
		if(!search(args->visited, current_url)){
			insert(args->visited, current_url);
			should_crawl = 1;
		}	
		pthread_mutex_unlock(args->visited_lock);
		if(should_crawl){
			printf("Crawling: %s\n", current_url);
			HttpData* page_data = download_page(current_url);
			if (page_data != NULL) {
				printf("Download riuscito. %zu bytes scaricati.\n", page_data->size);
				Queue *new_links = find_links(page_data->memory, current_url);
				if(new_links != NULL){
					pthread_mutex_lock(args->frontier_lock);
					while(!is_empty(new_links)){
						char *link = dequeue(new_links);
						if(link != NULL){
							enqueue(args->frontier, link);
							free(link);
						}
					}
					pthread_mutex_unlock(args->frontier_lock);
				}
				destroy_queue(new_links);
				free_http_data(page_data);
				pthread_mutex_lock(args->pages_crawled_lock);
				(*(args->pages_crawled))++;
				pthread_mutex_unlock(args->pages_crawled_lock);
			}
		}
		free(current_url);
	}
	return NULL;
}

int main(){


	char* seed_url = strdup("https://it.wikipedia.org/wiki/Crawler");
	
	Queue* frontier = create_queue();
	HashTable* visited = create_table(HASH_TABLE_SIZE);
	int pages_crawled = 0;
	enqueue(frontier, seed_url);
	free(seed_url);

	/*mutex initialization, as we're implementing threads we need to protect the shared resourses
	 * that could cause race conditions. In our case the frontier queue, the visited hashmap and the
	 * number of pages that has been already crawled. */

	pthread_mutex_t frontier_lock, visited_lock, pages_crawled_lock;
	pthread_mutex_init(&frontier_lock, NULL);
	pthread_mutex_init(&visited_lock, NULL);
	pthread_mutex_init(&pages_crawled_lock, NULL);

	WorkerArgs args;
	args.frontier = frontier;
	args.visited = visited;
	args.pages_crawled = &pages_crawled;
	args.max_pages = MAX_PAGES_TO_CRAWL;
	args.frontier_lock = &frontier_lock;
	args.visited_lock = &visited_lock;
	args.pages_crawled_lock = &pages_crawled_lock;

	pthread_t threads[THREAD_NUM];
	for(int i = 0; i < THREAD_NUM; i++){
		if(pthread_create(threads + i, NULL, &routine, &args) != 0){
			perror("Failed to create a thread");
			return 1;
		}
	}

	for(int i = 0; i < 16; i++){
		if(pthread_join(threads[i], NULL) != 0){
			return 2;
		}
	}
	
	print_table(visited);
	destroy_queue(frontier);
	destroy_table(visited);

	pthread_mutex_destroy(&frontier_lock);
	pthread_mutex_destroy(&visited_lock);
	pthread_mutex_destroy(&pages_crawled_lock);

	return 0;
}

