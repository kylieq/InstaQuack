/* Outline for topic queue */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "topic_queue.h"
#include "synch_bounded_queue.h"

int main() {
	int size = 30;
	struct SynchBoundedQueue *topic_queue = MallocTopicQueue(size);
	struct TopicEntry *entry = NULL;

	int check = 0;
	int ctr = 0;

	printf("Size of queue: %d\n", size);

	/* Fill queue & attempt to enqueue when full */
	for (ctr=0; ctr<size+5; ctr++) {
		entry = MakeEntry(ctr);
		check = Enqueue(topic_queue, entry);
		if (check > -1) {
			printf("Enqueued: %d\n", check);
		}
		else {
			printf("Enqueue Denied\n");
		}
	}

	/* Empty queue & attempt to dequeue when empty */
	for (ctr=0; ctr<size+5; ctr++) {
		int tail = GetBack(topic_queue);
		if (tail >= 0) {
			entry = GetEntry(topic_queue, tail);
			printf("Dequeued: %d\n", entry->entryNum);
			check = Dequeue(topic_queue, tail);
			if (check < 0) {
				printf("Dequeue Denied\n");
			}
		}
		else {
			printf("Dequeue Denied\n");
		}
	}
	FreeBoundedQueue(topic_queue);
	return 0;
}