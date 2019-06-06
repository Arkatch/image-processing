#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef __QUEUE3d__
#define __QUEUE3d__
typedef struct node3d{
    int32_t x, y, z;
    struct node3d *next;
}node3d;
typedef struct queue3d{
    int count;
    node3d *front;
    node3d *rear;
}queue3d;
void queue_start3d(queue3d *ptr){
    ptr->count = 0;
    ptr->front = NULL;
    ptr->rear = NULL;
}
bool is_empty_queue3d(queue3d *ptr){
    return ptr->count == 0;
}
void enqueue3d(queue3d *ptr, int32_t x, int32_t y, int32_t z){
    node3d *new_node = (node3d*)malloc(sizeof(node3d));
    new_node->x = x;
    new_node->y = y;
    new_node->z = z;
    if( is_empty_queue3d(ptr) ){
        ptr->front = new_node;
        ptr->rear = new_node;
    }
    else{
        ptr->rear->next = new_node;
        ptr->rear = new_node;
    }
    ptr->count++;
}
bool dequeue3d(queue3d *ptr, int32_t *x, int32_t *y, int32_t *z){
    if( is_empty_queue3d(ptr) )   
        return false;
    node3d *tmp = ptr->front;
    *x = ptr->front->x;
    *y = ptr->front->y;
    *z = ptr->front->z;
    ptr->front = ptr->front->next;
    ptr->count--;
    free(tmp);
    return true;
}
#endif
