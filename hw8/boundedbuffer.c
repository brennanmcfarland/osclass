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

#define N 10
#define BUFFER_SIZE 5

// Struct for condition variable
struct condition {
	sem_t sem; // Semaphore to be initialized to 0
	int count; // Count of threads waiting
};

/* Global variables are shared by the thread(s) */
sem_t mutex, next;
struct condition not_full, not_empty;
int next_count = 0;
int sum; // Global sum

int buffer[BUFFER_SIZE];

void *child(); /* child thread */

void cwait(struct condition *c); // Semaphore implementation of conditional wait
void cpost(struct condition *c); // Semaphore implementation of conditional signal

int main(int argc, char *argv[]) {
	pthread_t tid; // thread identifiers
	not_full.count = 0;
	not_empty.count = 0;
	sem_init(&mutex, 0, 1);
	sem_init(&(not_full.sem), 0, 1);
	sem_init(&(not_empty.sem), 0, 0);
	sem_init(&next, 0, 0);
	sum = 0;
	int start1 = 1;
	pthread_create(&tid, NULL, child, &start1); // Launch add

	// Parent does work
	int data[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100}; // Must have N values
	int i, in = 0; // Index for writing
	sem_wait(&mutex);
	cwait(&not_full);
	for(i = 0; i < N; i++) {
		//sleep((double) data[i] / 50); // Not required. Used for testing.
		buffer[in] = data[i];
		in = (in + 1) % BUFFER_SIZE;
	}
	if(next_count > 0)
		sem_post(&next);
	else
		sem_post(&mutex);
	cpost(&not_empty);

	// Join and print result
	pthread_join(tid, NULL);
	sem_destroy(&mutex);
	sem_destroy(&(not_full.sem));
	sem_destroy(&(not_empty.sem));
}

void *child() {
	int i, out = 0; // Index for reading
	sem_wait(&mutex);
	cwait(&not_empty);
	for(i = 0; i < N; i++) {
		int data = buffer[out];
		//sleep((double) data / 50); // Not required. Used for testing.
		printf("%d\n", data);
		out = (out + 1) % BUFFER_SIZE;
	}
	if(next_count > 0)
		sem_post(&next);
	else
		sem_post(&mutex);
	cpost(&not_full);
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
