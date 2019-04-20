#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef __QUEUE__
#define __QUEUE__
typedef struct node{
    int32_t x, y;
    struct node *next;
}node;
typedef struct queue{
    int count;
    node *front;
    node *rear;
}queue;
void queue_start(queue *ptr){
    ptr->count = 0;
    ptr->front = NULL;
    ptr->rear = NULL;
}
bool is_empty_queue(queue *ptr){
    return ptr->count == 0;
}
void enqueue(queue *ptr, int32_t x, int32_t y){
    node *new_node = (node*)malloc(sizeof(node));
    new_node->x = x;
    new_node->y = y;
    if( is_empty_queue(ptr) ){
        ptr->front = new_node;
        ptr->rear = new_node;
    }
    else{
        ptr->rear->next = new_node;
        ptr->rear = new_node;
    }
    ptr->count++;
}
bool dequeue(queue *ptr, int32_t *x, int32_t *y){
    if( is_empty_queue(ptr) )   
        return false;
    node *tmp = ptr->front;
    *x = ptr->front->x;
    *y = ptr->front->y;
    ptr->front = ptr->front->next;
    ptr->count--;
    free(tmp);
    return true;
}
#endif
