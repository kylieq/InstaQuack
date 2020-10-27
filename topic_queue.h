#ifndef TOPIC_QUEUE_H
#define TOPIC_QUEUE_H
#include <sys/time.h>
#include "synch_bounded_queue.h"
#define	CAPSIZE 10

struct TopicEntry {
	int entryNum;
	struct timeval *timeStamp;
	int pubID;
	char *photoURL; // URL to photo
	char *photoCaption[CAPSIZE]; // photo caption
};

struct SynchBoundedQueue *MallocTopicQueue(long size);

long long Enqueue(struct SynchBoundedQueue *queue,void *entry); 

int Dequeue(struct SynchBoundedQueue *queue,long long id);

void *GetEntry(struct SynchBoundedQueue *queue,long long id);

long long GetBack(struct SynchBoundedQueue *queue);

int ValidEntry(struct SynchBoundedQueue *queue,long long id); 

int IsFull(struct SynchBoundedQueue *queue); 

int IsEmpty(struct SynchBoundedQueue *queue); 

void FreeBoundedQueue(struct SynchBoundedQueue *queue); 

struct TopicEntry *MakeEntry();

#endif
