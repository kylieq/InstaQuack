#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "topic_queue.h"
#include "synch_bounded_queue.h"
#include "bounded_queue.h"
#include "file_ops.h"
#define NUMPROXIES 10
#define QUEUESIZE 10

pthread_cond_t cond = PTHREAD_COND_INITIALIZER; // Thread condition variable
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // Mutex

int DELTA = 0;

struct ArgStruct {
	char *filename;
	int index;
};

void *Subscriber(void *args){
	struct ArgStruct *arg = args;
	pthread_mutex_lock(&lock);
	pthread_cond_wait(&cond, &lock);
	printf("Proxy thread %u - type: Subscriber\n", (unsigned int)pthread_self());
	pthread_mutex_unlock(&lock); 
	return 0;
}

void *Publisher(void *args) {
	struct ArgStruct *arg = args;
	pthread_mutex_lock(&lock);
	pthread_cond_wait(&cond, &lock);
	printf("Proxy thread %u - type: Publisher\n", (unsigned int)pthread_self());
	pthread_mutex_unlock(&lock); 
	return 0;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Error: Expected 2 arguments. Given %d argument(s).\n", argc);
		return 0;
	}
	
	struct FileLines *file_lines = LoadAFile(argv[1]);
	struct LineArguments **line_arguments = malloc(file_lines->LineCount * sizeof(struct LineArguments *));

	for (int i=0; i<file_lines->LineCount; i++) {
		line_arguments[i] = malloc(sizeof(struct LineArguments));
		line_arguments[i]->args = malloc(10 * sizeof(char *));
	}

	// Tokenize each line (in file_lines) 
	for (int i=0; i<file_lines->LineCount; i++) {
		char *token = strtok(file_lines->Lines[i], " \n\r");
		int ctr = 0;
		while (token != NULL) {
			line_arguments[i]->args[ctr] = malloc(strlen(token)+1);
			strcpy(line_arguments[i]->args[ctr], token);
			token = strtok(NULL, " \"\n\r");
			ctr++;
		}
	}

	struct SynchBoundedQueue *topic_queues[QUEUESIZE];
	pthread_t subscriber_threads[NUMPROXIES];
	pthread_t publisher_threads[NUMPROXIES];
	char *topic_names[QUEUESIZE];
	char *subscriber_names[NUMPROXIES];
	char *publisher_names[NUMPROXIES];

	int topicCtr = 0, subCtr = 0, pubCtr = 0;

	for (int i=0; i<file_lines->LineCount; i++) {
		if (strcmp(line_arguments[i]->args[0], "create") == 0) {
			if (strcmp(line_arguments[i]->args[1], "topic") == 0) {
				int size = atoi(line_arguments[i]->args[4]);
				struct SynchBoundedQueue *topic_queue = MallocTopicQueue(size);
				topic_queues[topicCtr] = topic_queue;
				topic_names[topicCtr] = line_arguments[i]->args[3];
				topicCtr += 1;
			}
		}
		else if (strcmp(line_arguments[i]->args[0], "add") == 0) {
			struct ArgStruct *arg_struct = malloc(sizeof(struct ArgStruct *));
			if (strcmp(line_arguments[i]->args[1], "subscriber") == 0) {
				arg_struct->filename = line_arguments[i]->args[2];
				arg_struct->index = subCtr;
				subscriber_names[subCtr] = line_arguments[i]->args[2];
				pthread_create(&subscriber_threads[subCtr], NULL, Subscriber, arg_struct);
				subCtr += 1;
			}
			else if (strcmp(line_arguments[i]->args[1], "publisher") == 0) {
				arg_struct->filename = line_arguments[i]->args[2];
				arg_struct->index = pubCtr;
				publisher_names[pubCtr] = line_arguments[i]->args[2];
				pthread_create(&publisher_threads[pubCtr], NULL, Publisher, arg_struct);
				pubCtr += 1;
			}
		}
		else if (strcmp(line_arguments[i]->args[0], "query") == 0) {
			if (strcmp(line_arguments[i]->args[1], "subscribers") == 0) {
				printf("Subscribers:\n");
				for (int i=0; i<subCtr; i++){
					printf("Subscriber thread %d - %s\n", i, subscriber_names[i]);
				}
			}
			else if (strcmp(line_arguments[i]->args[1], "publishers") == 0) {
				printf("Publishers:\n");
				for (int i=0; i<pubCtr; i++){
					printf("Publisher thread %d - %s\n", i, publisher_names[i]);
				}
			}
		}
		else if (strcmp(line_arguments[i]->args[0], "delta") == 0) {
			DELTA = atoi(line_arguments[i]->args[1]);
		}
		else if (strcmp(line_arguments[i]->args[0], "start") == 0) {
			break;
		}
	}

	for (int i=0; i<10; i++) {
		printf("*****");
	}
	printf("\n");

	printf("Waiting for subscriber threads...\n");
	printf("Waiting for publisher threads...\n");

	sleep(5);
	pthread_cond_broadcast(&cond);

	int check = 0;
	for (int i=0; i<pubCtr; i++) {
		check += pthread_join(publisher_threads[i], NULL);
	}	
	if (check == 0) {
		printf("All publisher threads have successfully exited.\n");
	}

	check = 0;
	for (int i=0; i<subCtr; i++) {
		check += pthread_join(subscriber_threads[i], NULL);
	}
	if (check == 0) {
		printf("All subscriber threads have successfully exited.\n");
	}

	FreeFile(file_lines);
	return 0;
}