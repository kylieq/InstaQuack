/* Outline for topic queue */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "topic_queue.h"
#include "synch_bounded_queue.h"

struct SynchBoundedQueue *MallocTopicQueue(long size) {
	struct SynchBoundedQueue *result = NULL;
	result = SBQ_MallocBoundedQueue(size);
	return result;
}

long long Enqueue(struct SynchBoundedQueue *queue, void *entry) {
	long long result = SBQ_Enqueue(queue, entry);
	return result;
}

int Dequeue(struct SynchBoundedQueue *queue,long long id) {
	int result = SBQ_Dequeue(queue, id); 
	return result;
}

void *GetEntry(struct SynchBoundedQueue *queue,long long id) {
	void *result = SBQ_GetEntry(queue, id);
	return result;
}

long long GetBack(struct SynchBoundedQueue *queue) {
	long long result = SBQ_GetBack(queue);
	return result;
}

int ValidEntry(struct SynchBoundedQueue *queue,long long id) { 
	int result = SBQ_ValidEntry(queue, id);
	return result;
}

int IsFull(struct SynchBoundedQueue *queue) { 
	int result = SBQ_IsFull(queue); 
	return result;
}

int IsEmpty(struct SynchBoundedQueue *queue) { 
	int result = SBQ_IsEmpty(queue);
	return result;
}

void FreeBoundedQueue(struct SynchBoundedQueue *queue) { 
	SBQ_FreeBoundedQueue(queue);
}

struct TopicEntry *MakeEntry(int num) {
	struct TopicEntry *entry = NULL;
	entry = malloc(sizeof(struct TopicEntry));
	entry->entryNum = num;
	entry->timeStamp = NULL;
	entry->pubID = 0;
	entry->photoURL = NULL;
	for (int i=0; i<10; i++) {
		entry->photoCaption[i] = NULL;
	}
	return entry;
}
