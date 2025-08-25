#include "queue.h"

/*This funcion allocates the memory for the queue struct and returns the pointer to the struct. */

Queue* create_queue() {
	Queue *new_queue= (Queue *)malloc(sizeof(Queue));
	new_queue->head = NULL;
	new_queue->tail = NULL;
	return new_queue;
}

/*This function takes a pointer to a queue and a pointer to an url and enqueues the url
 * allocating the memory for both the new node and the url*/

void enqueue(Queue *q, const char *url) {
	Node *new_node = (Node *)malloc(sizeof(Node));
	new_node->url = malloc(strlen(url)+1);
	strcpy(new_node->url, url);
	new_node->next = NULL;
	if (is_empty(q)) {
		q->head = new_node;
		q->tail = new_node;
	}else {
		q->tail->next = new_node;
		q->tail= new_node;
	}
}

/*This function takes a queue and pops the first url stored in the head node.  */

char* dequeue(Queue *q) {
	if (is_empty(q)) return NULL;

	Node *temp_node = q->head;
	char *url1 = temp_node->url;
	q->head = temp_node->next;

	if (is_empty(q)) q->tail = NULL;

	free(temp_node);
	
	return url1;
}

//this function returns 1 if the queue is empty and 0 if it's not.

int is_empty(Queue *q) {
	return (q->head == NULL);
}

/*This funtion frees all the memory required to store the input queue. */

void destroy_queue(Queue *q){
	while (!is_empty(q)){
		char *url = dequeue(q);
		free(url);
	}
	free(q);
}

/*This function prints a queue node by node. */

void print_queue(Queue *q) {
    	printf("--- Start of the queue ---\n");

    	if (q == NULL || q->head == NULL) {
        	printf("-> The queue is empty.\n");
    	} else {
        	int count = 1;
        	for (Node *current = q->head; current != NULL; current = current->next) {
            		printf("  %d: %s\n", count, current->url);
            		count++;
        	}
    	}
    
    	printf("--- End of the queue ---\n\n");
}

