#include "queue.h"

Queue* creare_queue() {

	Queue *new_queue= (Queue *)malloc(sizeof(Queue));
	new_queue->head = NULL;
	new_queue->tail = NULL;
	return new_queue;
}

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

char* dequeue(Queue *q) {
	
	if (is_empty(q)) return NULL;
	
	Node *temp_node = q->head;

	char *url1 = temp_node->url;
	
	q->head = temp_node->next;

	if (is_empty(q)) q->tail = NULL;
	
	free(temp_node);

	return url1;
}



int is_empty(Queue *q) {
	return (q->head == NULL);
}

void destroy_queue(Queue *q){
	 
	while (!is_empty(q)){
		char *url = dequeue(q);
		free(url);
	}
	free(q);
}











