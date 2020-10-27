#ifndef SYNCH_BOUNDED_QUEUE_H
#define SYNCH_BOUNDED_QUEUE_H
#include <pthread.h> 
#include <unistd.h>
#include "bounded_queue.h"

struct SynchBoundedQueue {
	struct BoundedQueue *queue;
	pthread_mutex_t lock; 
};

struct SynchBoundedQueue *SBQ_MallocBoundedQueue(long size);

long long SBQ_Enqueue(struct SynchBoundedQueue *queue,void *entry); 

int SBQ_Dequeue(struct SynchBoundedQueue *queue,long long id);

void *SBQ_GetEntry(struct SynchBoundedQueue *queue,long long id);

long long SBQ_GetBack(struct SynchBoundedQueue *queue);

int SBQ_ValidEntry(struct SynchBoundedQueue *queue,long long id);

int SBQ_IsFull(struct SynchBoundedQueue *queue);

int SBQ_IsEmpty(struct SynchBoundedQueue *queue);

void SBQ_FreeBoundedQueue(struct SynchBoundedQueue *queue);

#endif
