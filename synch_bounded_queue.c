/* Functionality to handle locks */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <unistd.h>
#include "bounded_queue.h"
#include "synch_bounded_queue.h"

struct SynchBoundedQueue *SBQ_MallocBoundedQueue(long size) {
	struct SynchBoundedQueue *result = NULL;
    result = malloc(sizeof(struct SynchBoundedQueue));
	pthread_mutex_init(&(result->lock), NULL);
	result->queue = BQ_MallocBoundedQueue(size);
	return result; 
}

long long SBQ_Enqueue(struct SynchBoundedQueue *queue,void *entry) {
	pthread_mutex_lock(&(queue->lock)); 
	long long result = BQ_Enqueue(queue->queue,entry);
	pthread_mutex_unlock(&(queue->lock)); 
	if (result < 0) return -1;
	return result;
}

int SBQ_Dequeue(struct SynchBoundedQueue *queue,long long id) {
	pthread_mutex_lock(&(queue->lock)); 
	int result = BQ_Dequeue(queue->queue,id);
	pthread_mutex_unlock(&(queue->lock)); 
	if (result < 0) return -1;
	return result;
}

void *SBQ_GetEntry(struct SynchBoundedQueue *queue,long long id) {
	pthread_mutex_lock(&(queue->lock)); 
	void *result = BQ_GetEntry(queue->queue,id);
	pthread_mutex_unlock(&(queue->lock)); 
	return result;
}

long long SBQ_GetBack(struct SynchBoundedQueue *queue) {
	pthread_mutex_lock(&(queue->lock)); 
	long long result = BQ_GetBack(queue->queue);
	pthread_mutex_unlock(&(queue->lock)); 
    if (result < 0) return -1;
	return result;
}

int SBQ_ValidEntry(struct SynchBoundedQueue *queue,long long id) {
    int result = 0;  
    pthread_mutex_lock(&(queue->lock)); 
    result = BQ_ValidEntry(queue->queue,id);
    pthread_mutex_unlock(&(queue->lock)); 
    return result;
}

int SBQ_IsFull(struct SynchBoundedQueue *queue) {
    int result = 0;
    pthread_mutex_lock(&(queue->lock)); 
    result = BQ_IsFull(queue->queue);
    pthread_mutex_unlock(&(queue->lock)); 
    return result;
}

int SBQ_IsEmpty(struct SynchBoundedQueue *queue) {
    int result = 0;
    pthread_mutex_lock(&(queue->lock)); 
    result = BQ_IsEmpty(queue->queue);
    pthread_mutex_unlock(&(queue->lock)); 
    return result;
}

void SBQ_FreeBoundedQueue(struct SynchBoundedQueue *queue) {
    BQ_FreeBoundedQueue(queue->queue);
    queue->queue = NULL;
    pthread_mutex_destroy(&(queue->lock));
    free(queue);
}