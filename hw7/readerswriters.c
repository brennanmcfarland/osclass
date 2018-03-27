#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 16
#define STRING_SIZE 12

/* PROTOTYPES */
void make_thread(pthread_t tid, pthread_attr_t attr, void *(*func)());
void *reader(int initversion);
int read_from_buffer(int out);
void *writer();
void write_to_buffer(char *value);

/* GLOBAL VARIABLES */
char buffer[BUFFER_SIZE];
int version;
sem_t rw_mutex;
sem_t mutex;
int read_count = 0;
int in = 0;

int main()
{
  //we start able to read/write
  sem_init(&rw_mutex, 0, 1);
  sem_init(&mutex, 0, 1);

  //run the writer in the parent thread and the readers in the child threads
  pthread_t tid1, tid2;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_create(&tid1, &attr, reader, 0);
  pthread_create(&tid2, &attr, reader, 0);
  writer();
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
  //guaranteed both threads are done with the critical section at this point
  sem_destroy(&rw_mutex);
  sem_destroy(&mutex);

  return 0;
}

void *reader(int initversion)
{
  int out = 0;
  int myversion = initversion;
  while(myversion < 3)
  {
    if(myversion < version)
    {
      out = read_from_buffer(out);
      myversion++;
    }
  }
}

int read_from_buffer(int out)
{
  //wait for an opportunity to read
  sem_wait(&mutex);
  read_count++;
  if(read_count == 1)
    sem_wait(&rw_mutex);
  sem_post(&mutex);

  //read
  char nextstring[STRING_SIZE];
  int i;
  for(i=0; i<STRING_SIZE; i++)
  {
    nextstring[i] = buffer[out];
    out = (out+1)%BUFFER_SIZE;
    if(nextstring[i] == '\0') { break; }
  }
  printf("%lld : %s\n", pthread_self(), nextstring);
  fflush(stdout);
  //signal reading is done
  sem_wait(&mutex);
  read_count--;
  if(read_count == 0)
    sem_post(&rw_mutex);
  sem_post(&mutex);
  return out;
}

void *writer()
{
  write_to_buffer("string A");
  sleep(1);
  write_to_buffer("string B");
  sleep(1);
  write_to_buffer("string C");
  sleep(1);
}

void write_to_buffer(char *value)
{
  //wait for an opportunity to write
  sem_wait(&rw_mutex);
  //write
  int i;
  //manually printing char by char because it's a circular array
  for(i=0; i<STRING_SIZE; i++)
  {
    //if it's the null terminator, break
    buffer[in] = value[i];
    in = (in+1)%BUFFER_SIZE;
    if(value[i] == '\0') { break; }
  }
  //increment version
  version++;
  //signal writing is done
  sem_post(&rw_mutex);
}
