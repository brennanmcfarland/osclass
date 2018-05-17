#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 16

/* PROTOTYPES */
void make_thread(pthread_t tid, pthread_attr_t attr, void *(*func)());
void *producer();
void *consumer();

/* GLOBAL VARIABLES */
char buffer[BUFFER_SIZE];
int in = 0;
int out = 0;
int N = 256;
sem_t empty, full, mutex;

int main()
{
  //the buffer starts empty
  sem_init(&empty, 0, 1);
  sem_init(&full, 0, 0);
  sem_init(&mutex, 0, 1);

  //run the consumer in the parent thread and the producer in the child thread
  pthread_t tid;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_create(&tid, &attr, producer, NULL);
  consumer();
  pthread_join(tid, NULL);
  printf("\n");

  //destroy semaphores
  sem_destroy(&empty);
  sem_destroy(&full);
  sem_destroy(&mutex);

  return 0;
}

void *producer()
{
  char vals[N];
  int i;
  for(i=0; i<N; i++){vals[i] = i%10 + '0';}
  int next = 0;
  while(next < N)
  {
    //wait till the buffer is empty
    sem_wait(&empty);
    sem_wait(&mutex);
    buffer[in] = vals[next];
    in = (in+1)%BUFFER_SIZE;
    //indicate the buffer is full
    sem_post(&mutex);
    sem_post(&full);
    next++;
  }
}

void *consumer()
{
  int next = 0;
  char nextval;
  while(next < N)
  {
    //wait till the buffer is full
    sem_wait(&full);
    sem_wait(&mutex);
    nextval = buffer[out];
    printf("%c ", nextval);
    fflush(stdout);
    out = (++out)%BUFFER_SIZE;
    //indicate the buffer is empty
    sem_post(&mutex);
    sem_post(&empty);
    next++;
  }
}
