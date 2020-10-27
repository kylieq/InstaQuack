#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "topic_queue.h"
#include "synch_bounded_queue.h"

pthread_cond_t cond = PTHREAD_COND_INITIALIZER; // Thread condition variable
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // Mutex

int COUNTER = 0;

struct SynchBoundedQueue *topic_queue;

void *Subscriber(void *arg) {
	pthread_mutex_lock(&lock);
	pthread_cond_wait(&cond, &lock); // block until condition var cond is broadcast to
	printf("Proxy thread %u - type: %s\n", (unsigned int)pthread_self(), (char*)arg);

	int check = 0;
	int tail = GetBack(topic_queue);
	if (tail >= 0) {
		struct TopicEntry *entry = GetEntry(topic_queue, tail);
		printf("Dequeued: %d\n", entry->entryNum);
		check = Dequeue(topic_queue, tail);
		if (check < 0) {
			printf("Dequeue Denied\n");
		}
	}
	else {
		printf("Dequeue Denied\n");
	}

	pthread_mutex_unlock(&lock);
	return 0;
}

void *Publisher(void *arg) {
	pthread_mutex_lock(&lock);
	pthread_cond_wait(&cond, &lock); // block until condition var cond is broadcast to
	printf("Proxy thread %u - type: %s\n", (unsigned int)pthread_self(), (char*)arg);
	
	int check = 0;
	struct TopicEntry *entry = MakeEntry(COUNTER);
	check = Enqueue(topic_queue, entry);
	if (check > -1) {
		printf("Enqueued: %d\n", check);
	}
	else {
		printf("Enqueue Denied\n");
	}
	pthread_mutex_unlock(&lock);
	COUNTER++;
	return 0;
}

int main() {
	int size = 30;
	topic_queue = MallocTopicQueue(size);

	char *arg1 = "Publisher";
	char *arg2 = "Subscriber";

	// Create threads
	pthread_t publisher, subscriber;

	for (int i=0; i<5; i++) {
		pthread_create(&publisher, NULL, Publisher, arg1);
	}

	for (int i=0; i<10; i++) {
		pthread_create(&subscriber, NULL, Subscriber, arg2);
	}

	// Unblock threads blocked on condition var cond
	pthread_cond_broadcast(&cond);
	sleep(5);

	// Wait for threads to terminate
	int check = 0;
	check += pthread_join(subscriber, NULL);
	check += pthread_join(publisher, NULL);

	if (check == 0) {
		printf("All threads have successfully exited.\n");
	}

	FreeBoundedQueue(topic_queue);
	return 0;
}