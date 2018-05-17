#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

#define BUFFER_SIZE 20

struct condition {
  sem_t sem;
  int count;
};

sem_t rw_mutex, mutex, next;
struct condition notreading, notwriting;
int next_count = 0;
char buffer[BUFFER_SIZE];
int buffer_version = 0;
int read_count = 0;

void cwait(struct condition *c); // Wait for condition variable
void cpost(struct condition *c); // Signl/post condition variable
void *reader(); // Reader thread
void writer(char string[]); // Writer thread

int main(int argc, char *argv[])
{
  notreading.count = 0;
  sem_init(&(notreading.sem), 0, 0);
  sem_init(&next, 0, 0);
  sem_init(&mutex, 0, 1);

  // Prepare the reader
  pthread_t tid1, tid2;
  pthread_create(&tid1, NULL, reader, NULL);
  pthread_create(&tid2, NULL, reader, NULL);

  // Writer
  //the writer gets starved at this line, so it's either waiting on mutex or notreading
  writer("Roses are red");
  writer("Violets are blue");
  writer("Synchronization is");
  writer("Quite fun to do");

  // Join
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
  sem_destroy(&mutex);
  sem_destroy(&next);
  sem_destroy(&(notreading.sem));
}

void *reader() {
  int old_version = 0;
  pthread_t tid = pthread_self();
  int i;
  for (i = 0; i < 5; i++) {
    sem_wait(&mutex);
    read_count++;
    if (read_count == 1)
    {
      printf("waiting on notwriting...%d\n", notwriting);
      cwait(&notwriting);
      printf("notwriting acquired\n");
    }
    sem_post(&mutex);
    printf("%d: %s\n", tid, buffer);
    fflush(stdout); // Not required, but may help
    sem_wait(&mutex);
    read_count--;
    if (read_count == 0)
      cpost(&notreading);
    if (next_count > 0)
      sem_post(&next);
    else
      sem_post(&mutex);
  }
  pthread_exit(0);
}

void writer(char string[]) {
  printf("waiting for mutex...\n");
  sem_wait(&mutex);
  printf("mutex acquired\n");
  if (read_count > 0)
  {
    //it always starves here
    printf("waiting for notreading...\n");
    cwait(&notreading);
    printf("notreading acquired\n");
  }
  sprintf(buffer, "%s", string);
  cpost(&notwriting);
  if (next_count > 0)
    sem_post(&next);
  else
    sem_post(&mutex);
}

void cwait(struct condition *c) {
  c->count++;
  if (next_count > 0)
    sem_post(&next);
  else
    sem_post(&mutex);
  sem_wait(&(c->sem));
  c->count--;
}

void cpost(struct condition *c) {
  next_count++;
  sem_post(&(c->sem));
  sem_wait(&next);
  next_count--;
}
