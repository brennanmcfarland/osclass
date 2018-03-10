#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Prototypes
double shubert(double x1, double x2);
void shubert_local_minimum(double minx1, double maxx1, double minx2, double maxx2);
void *child(); //thread

double min;
sem_t sem;

int main()
{
  //initialize the semaphore
  if(sem_init(&sem, 0, 1) < 0)
  {
    fprintf(stderr, "Unable to initialize semaphore\n");
    exit(1);
  }

  //initialize thread to calculate the local min for one half
  pthread_t tid;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_create(&tid, &attr, child, NULL);

  //meanwhile, calculate the local min for the other half
  double x1min = -2;
  double x1max = 0;
  shubert_local_minimum(x1min, x1max, -2, 2);
  printf("Parent x1 = %.1f to %.1f\n", x1min, x1max);

  //wait for the child thread to exit and get its local min
  pthread_join(tid, NULL);
  printf("Global min: %.2f\n", min);

  sem_destroy(&sem); //destroy the semaphore
  return 0;
}

// Computer Shubert local minimum for the given range
void shubert_local_minimum(double minx1, double maxx1, double minx2, double maxx2)
{
  double x1, x2, y = 0;
  for (x1 = minx1; x1 <= maxx1; x1 += 0.001)
  {
    for (x2 = minx2; x2 <= maxx2; x2 += 0.001)
    {
      y = shubert(x1, x2);
      if (y < min)
      {
        sem_wait(&sem); //wait for semaphore availability
        min = y;
        sem_post(&sem); //post that the semaphore is available
      }
    }
  }
}

// Compute Shubert value for x1,x2 combination
double shubert(double x1, double x2)
{
	double sum1 = 0;
	double sum2 = 0;
	int i;
	for (i = 1; i <= 5; i++) {
		sum1 += i * cos((i + 1) * x1 + i);
		sum2 += i * cos((i + 1) * x2 + i);
	}
	return sum1 * sum2;
}

void *child()
{
  double x1min = 0;
  double x1max = 2;
  shubert_local_minimum(x1min, x1max, -2, 2);
  printf("Child x1 = %.1f to %.1f\n", x1min, x1max);
  pthread_exit(0);
}
