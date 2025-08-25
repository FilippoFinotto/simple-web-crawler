#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Node{
        char *url;
        struct Node *next;
}Node;

typedef struct{
        struct Node *head;
        struct Node *tail;
}Queue;

Queue* create_queue();
void enqueue(Queue *q, const char *url);
char* dequeue(Queue *q);
int is_empty(Queue *q);
void destroy_queue(Queue *q);
void print_queue(Queue *q);

#endif
