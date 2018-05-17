/* Chris Fietkiewicz. Demonstrates a condition variable using semaphores.
Based on algorithm from Operating System Concepts by Silberschatz et al.
Does the following steps:
1. Creates a child thread that waits on condition varible "x"
2. Sleeps for 1 second
3. Signals child usisng condition variable
4. Child resumes and finishes
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

/* Global variables are shared by the thread(s) */
struct condition x;

void cwait(struct condition *c); // Semaphore implementation of conditional wait
void cpost(struct condition *c); // Semaphore implementation of conditional signal
void *child(); // Child thread that will wait on condition variable

int main(int argc, char *argv[]) {
	pthread_t tid; // thread identifiers
	x.count = 0; // Initialize condition variable count (nobody waiting)
	sem_init(&(x.sem), 0, 0); // Initialize condition variable semaphore
	pthread_create(&tid, NULL, child, NULL); // Launch child
	printf("Parent sleeping...\n ");
	fflush(stdout);
	sleep(1);
	printf("Parent signaling...\n ");
	fflush(stdout);
	cpost(&x); // Signal for one thread to compute
	printf("Parent Done!\n ");
	fflush(stdout);
	pthread_join(tid, NULL); // Block until child is finished
	sem_destroy(&(x.sem));
}

// Child thread that will wait on condition variable
void *child() {
	printf("Child waiting on x...\n ");
	cwait(&x); // Wait for condition signal
	printf("Child done waiting on x...\n ");
	fflush(stdout);
	pthread_exit(0);
}

// Semaphore implementation of conditional wait
void cwait(struct condition *c) {
	c->count++;
	sem_wait(&(c->sem));
	c->count--;
}

// Semaphore implementation of conditional signal
void cpost(struct condition *c) {
	if (c->count > 0) {
		sem_post(&(c->sem));
	}
}
