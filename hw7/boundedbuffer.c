#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 4

/* PROTOTYPES */
void make_thread(pthread_t tid, pthread_attr_t attr, void *(*func)());
void *producer();
void *consumer();

/* GLOBAL VARIABLES */
int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;
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
  make_thread(tid, attr, producer);
  consumer();
  return 0;
}

void make_thread(pthread_t tid, pthread_attr_t attr, void *(*func)())
{
  pthread_attr_init(&attr);
  pthread_create(&tid, &attr, func, NULL);
}

void *producer()
{
  //TODO: feed it real values and not just these test vals
  int vals[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int next = 0;
  while(1)
  {
    printf("got to producer"); fflush(stdout);
    //wait till the buffer is empty
    sem_wait(&empty);
    sem_wait(&mutex);
    printf("in producer critical section"); fflush(stdout);
    buffer[in] = vals[next];
    in = (++in)%BUFFER_SIZE;
    sem_post(&mutex);
    sem_post(&empty);
  }
}

void *consumer()
{
  int nextval;
  while(1)
  {
    printf("got to consumer"); fflush(stdout);
    //wait till the buffer is full
    //silly!  we never decrement full, do we, because it starts out empty
    sem_wait(&full); //TODO: probably need to have this not block if the producer is done, too (won't fill up then)
    sem_wait(&mutex);
    printf("in consumer critical section"); fflush(stdout);
    nextval = buffer[out];
    printf("%u\n", nextval);
    fflush(stdout);
    out = (++out)%BUFFER_SIZE;
    sem_post(&mutex);
    sem_post(&full);
  }
}
