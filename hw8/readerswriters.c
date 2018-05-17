/* Chris Fietkiewicz. Demonstrates monitor solution using semaphores.
Based on algorithm from Operating System Concepts by Silberschatz et al.
Does the following steps:
1. Creates 2 threads for "add" function.
   Each thread waits on cond. var. and computes a sum (1-50 and 51-100).
2. Creates 3rd thread for "total" function which signals "add" threads.
3. 3rd thread sleeps for 1 sec to give threads some time to finish and prints sum.
*/
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h> // For sleep

// Struct for condition variable
struct condition {
	sem_t sem; // Semaphore to be initialized to 0
	int count; // Count of threads waiting
};

#define BUFFER_SIZE 20

/* Global variables are shared by the thread(s) */
sem_t rw_mutex, mutex;
char buffer[BUFFER_SIZE];
int buffer_version = 0;
int read_count = 0;

void *child(); /* child thread */

sem_t mutex, next;
struct condition not_reading;
int next_count = 0;

void cwait(struct condition *c); // Semaphore implementation of conditional wait
void cpost(struct condition *c); // Semaphore implementation of conditional signal
void *total(); // Computes total for all subranges

int main(int argc, char *argv[]) {
	pthread_t tid1, tid2; // thread identifiers
	not_reading.count = 0;
	sem_init(&mutex, 0, 1);
	sem_init(&(not_reading.sem), 0, 0);
	sem_init(&next, 0, 0);
	int start1 = 1;
	pthread_create(&tid1, NULL, child, &start1); // Launch child
	int start2 = 51;
	pthread_create(&tid2, NULL, child, &start2); // Launch child

	// Writer
	sem_wait(&mutex);
	cwait(&not_reading);
	sprintf(buffer, "%s", "Roses are red");
	buffer_version = 1;
	sem_post(&mutex);
	sleep(1);
	sem_wait(&mutex);
	cwait(&not_reading);
	sprintf(buffer, "%s", "Violets are blue");
	buffer_version = 1;
	sem_post(&mutex);
	sleep(1);
	sem_wait(&mutex);
	cwait(&not_reading);
	sprintf(buffer, "%s", "Synchronization is");
	buffer_version = 1;
	sem_post(&mutex);
	sleep(1);
	sem_wait(&mutex);
	cwait(&not_reading);
	sprintf(buffer, "%s", "Quite fun to do");
	buffer_version = 1;
	sem_post(&mutex);
	sleep(1);

	// Join and print result
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	sem_destroy(&mutex);
	sem_destroy(&(not_reading.sem));
}

// Reader
void *child() {
	int j = 0, old_version = 0;
	pthread_t tid = pthread_self();
	while (old_version != 4) {
		sem_wait(&mutex);
		cwait(&not_reading);
		read_count++;
		sem_post(&mutex);
		if (buffer_version != old_version) {
			printf("%d: %s\n", tid, buffer);
			fflush(stdout); // Not required, but may help
			old_version = buffer_version;
		}
		sem_wait(&mutex);
		read_count--;
		if (read_count == 0)
			cpost(&not_reading);
		sem_post(&mutex);
	}
	pthread_exit(0);
}

// Semaphore implementation of conditional wait
void cwait(struct condition *c) {
	c->count++;
	if (next_count > 0)
		sem_post(&next);
	else
		sem_post(&mutex);
	sem_wait(&(c->sem));
	c->count--;
}

// Semaphore implementation of conditional signal
void cpost(struct condition *c) {
	if (c->count > 0) {
		next_count++;
		sem_post(&(c->sem));
		sem_wait(&next);
		next_count--;
	}
}
