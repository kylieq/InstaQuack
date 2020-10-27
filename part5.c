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

struct SynchBoundedQueue *topic_queues[QUEUESIZE];
char *topic_names[QUEUESIZE];

int DELTA = 0;
int COUNTER = 0;
int TOPIC_COUNT = 0;

struct ArgStruct {
	char *filename;
	int index;
};

char *subnum[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};

void *Subscriber(void *args){
	struct ArgStruct *arg = args;
	pthread_mutex_lock(&lock);
	pthread_cond_wait(&cond, &lock);

	struct FileLines *file_lines = LoadAFile((char *)arg->filename);
	struct LineArguments **line_arguments = malloc(file_lines->LineCount * sizeof(struct LineArguments *));

	for (int i=0; i<file_lines->LineCount; i++) {
		line_arguments[i] = malloc(sizeof(struct LineArguments));
		line_arguments[i]->args = malloc(10 * sizeof(char *));
	}

	// Tokenize each line (in file_lines) 
	int get_count = 0;
	for (int i=0; i<file_lines->LineCount; i++) {
		char *token = strtok(file_lines->Lines[i], " \n\r");
		int ctr = 0;
		while (token != NULL) {
			if (strcmp(token, "get") == 0) {
				get_count++;
			}
			line_arguments[i]->args[ctr] = malloc(strlen(token)+1);
			line_arguments[i]->count++;
			strcpy(line_arguments[i]->args[ctr], token);
			token = strtok(NULL, " \"\n\r");
			ctr++;
		}
	}

	struct TopicEntry *entries[get_count];
	int SE_CTR = 0;

	for (int i=0; i<get_count; i++) {
		entries[i] = NULL;
	}

	for (int i=0; i<file_lines->LineCount; i++) {
		if (strcmp(line_arguments[i]->args[0], "get") == 0) {
			printf("Proxy thread %u - type: Subscriber (%d) - Executed command: get\n", (unsigned int)pthread_self(), arg->index);
			int topic_num = atoi(line_arguments[i]->args[1]);
			int check = 0;
			int tail = GetBack(topic_queues[topic_num]);
			if (tail >= 0) {
				struct TopicEntry *entry = GetEntry(topic_queues[topic_num], tail);
				check = Dequeue(topic_queues[topic_num], tail);
				if (check < 0) {
					printf("Dequeue Denied\n");
				}
				else {
					printf("Dequeued from topic \"%s\": ", topic_names[topic_num]);
					int cap_ctr = 0;
					while (entry->photoCaption[cap_ctr] != NULL) {
						printf("%s ", entry->photoCaption[cap_ctr]);
						cap_ctr++;
					}
					printf("\n");

					// Save dequeued entry 
					entries[SE_CTR] = entry;
					SE_CTR++;
				}
			}
			else {
				printf("Dequeue Denied\n");
			}
			continue;
		}
		else if (strcmp(line_arguments[i]->args[0], "sleep") == 0) {
			printf("Proxy thread %u - type: Subscriber (%d) - Executed command: sleep\n", (unsigned int)pthread_self(), arg->index);
			int num_ms = atoi(line_arguments[i]->args[1]);
			sleep(num_ms);
		}
		else if (strcmp(line_arguments[i]->args[0], "stop") == 0) {
			printf("Proxy thread %u - type: Subscriber (%d) - Executed command: stop\n", (unsigned int)pthread_self(), arg->index);
			break;
		}
	}

	char *thread = "Subscriber";
	char filename[100];
	strcat(filename, thread);
	strcat(filename, subnum[arg->index]);
	strcat(filename, ".html");
	strcat(filename, "\0");
	FILE *file = fopen(filename, "w");

	fprintf(file, "<!DOCTYPE html><html><title>HTML_SUBSCRIBER_FILENAME</title>");
	fprintf(file, "<style>table, th, td {border: 1px solid black;border-collapse: collapse;}th, td {padding: 5px;}th {text-align: left;}</style>");
	fprintf(file, "</head><body>");
	fprintf(file, "<h1>Subscriber: %s</h1>", (char *)arg->filename);

	for (int i=0; i<TOPIC_COUNT; i++) {
		int idx = 0;
		int current = i;
		fprintf(file, "<h2>Topic Name: %s</h2>", topic_names[i]);
		fprintf(file, "<table style=\"width:100%%\" align=\"middle\">");
		fprintf(file, "<tr><th>CAPTION</th><th>PHOTO-URL</th></tr>");
		while (entries[idx] != NULL) {
			if (entries[idx]->entryNum == current) {
				fprintf(file, "<tr>");
				fprintf(file, "<td>");
				int cap_ctr = 0;
				while (entries[idx]->photoCaption[cap_ctr] != NULL) {
					fprintf(file, "%s ", entries[idx]->photoCaption[cap_ctr]);
					cap_ctr++;
				}
				fprintf(file, "</td>");
				fprintf(file, "<td>%s</td>", entries[idx]->photoURL);
				fprintf(file, "</tr>");
			}
			idx++;
		}
		fprintf(file, "</table>");
	}

	fprintf(file, "</body></html>");
	fclose(file);

	pthread_mutex_unlock(&lock); 
	return 0;
}

void *Publisher(void *args) {
	struct ArgStruct *arg = args;
	pthread_mutex_lock(&lock);
	pthread_cond_wait(&cond, &lock);

	struct FileLines *file_lines = LoadAFile((char *)arg->filename);
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
			line_arguments[i]->count++;
			strcpy(line_arguments[i]->args[ctr], token);
			token = strtok(NULL, " \"\n\r");
			ctr++;
		}
	}

	for (int i=0; i<file_lines->LineCount; i++) {
		if (strcmp(line_arguments[i]->args[0], "put") == 0) {
			printf("Proxy thread %u - type: Publisher (%d) - Executed command: put\n", (unsigned int)pthread_self(), arg->index);
			int topic_num = atoi(line_arguments[i]->args[1]);
			int check = 0;

			struct TopicEntry *entry = MakeEntry(COUNTER);
			entry->pubID = (unsigned int)pthread_self();
			entry->photoURL = strdup(line_arguments[i]->args[2]);
			entry->entryNum = topic_num;

			int args_idx = 3;
			int cap_idx = 0;
			while (args_idx < line_arguments[i]->count) {
				entry->photoCaption[cap_idx] = strdup(line_arguments[i]->args[args_idx]);
				cap_idx++;
				args_idx++;
			}

			check = Enqueue(topic_queues[topic_num], entry);
			if (check > -1) {
				printf("Enqueued to topic \"%s\": ", topic_names[topic_num]);
				for (int i=0; i<cap_idx; i++) {
					printf("%s ", entry->photoCaption[i]);
				}
				printf("\n");
				COUNTER++;
			}
			else {
				printf("Enqueue Denied\n");
			}
		}
		else if (strcmp(line_arguments[i]->args[0], "sleep") == 0) {
			printf("Proxy thread %u - type: Publisher (%d) - Executed command: sleep\n", (unsigned int)pthread_self(), arg->index);
			int num_ms = atoi(line_arguments[i]->args[1]);
			sleep(num_ms);
		}
		else if (strcmp(line_arguments[i]->args[0], "stop") == 0) {
			printf("Proxy thread %u - type: Publisher (%d) - Executed command: stop\n", (unsigned int)pthread_self(), arg->index);
			break;
		}
	}
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

	pthread_t subscriber_threads[NUMPROXIES];
	pthread_t publisher_threads[NUMPROXIES];
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
				TOPIC_COUNT += 1;
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
		}
	}

	FreeFile(file_lines);
	return 0;
}