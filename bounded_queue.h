#ifndef BOUNDED_QUEUE_H
#define BOUNDED_QUEUE_H

struct BoundedQueue {
	void **buffer;
	long long head;
	long long tail;
	int size;
};

struct BoundedQueue *BQ_MallocBoundedQueue(long size); 

long long BQ_Enqueue(struct BoundedQueue *queue,void *entry); 

int BQ_Dequeue(struct BoundedQueue *queue,long long id);

void *BQ_GetEntry(struct BoundedQueue *queue,long long id);

long long BQ_GetBack(struct BoundedQueue *queue); 

int BQ_ValidEntry(struct BoundedQueue *queue,long long id); 

int BQ_IsFull(struct BoundedQueue *queue);

int BQ_IsEmpty(struct BoundedQueue *queue); 

void BQ_FreeBoundedQueue(struct BoundedQueue *queue); 

#endif
