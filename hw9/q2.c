#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>


// Prototypes
double shubert(double x1, double x2);
void shubert_local_minimum(double minx1, double maxx1, double minx2, double maxx2);
void *child(double x1range[]); //thread

double min;
sem_t sem;

int main(int argc, char *argv[])
{
  if(argc < 2)
  {
    printf("Must specify number of threads\n");
    return 1;
  }

  struct timeval start_time, stop_time, elapsed_time;  // timers
	gettimeofday(&start_time,NULL); // Unix timer

  //initialize thread ids
  int num_threads = atoi(argv[1]);
  pthread_t tids[num_threads];
  int threadnum;
  for(threadnum = 0; threadnum < num_threads; threadnum++)
  {
    tids[num_threads] = 0;
  }

  //initialize the semaphore
  if(sem_init(&sem, 0, 1) < 0)
  {
    fprintf(stderr, "Unable to initialize semaphore\n");
    exit(1);
  }

  double totalrange[] = {-2, 2};
  double stepsize = (totalrange[1]-totalrange[0])/num_threads;
  double childranges[num_threads+1]; //starting and ending points
  childranges[0] = totalrange[0];
  for(threadnum = 1; threadnum < num_threads+1; threadnum++)
  {
    childranges[threadnum] = childranges[0] + stepsize * threadnum;
  }
  //only the child threads do computation
  for(threadnum = 0; threadnum < num_threads; threadnum++)
  {
    //initialize thread to calculate the local min for region
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&(tids[threadnum]), &attr, child, &childranges[threadnum]);
  }

  //wait for each child thread to exit and get its local min
  for(threadnum = 0; threadnum < num_threads; threadnum++)
  {
    pthread_join(tids[threadnum], NULL);
  }

  printf("Global min: %.2f\n", min);
  sem_destroy(&sem); //destroy the semaphore

  gettimeofday(&stop_time,NULL);
	timersub(&stop_time, &start_time, &elapsed_time); // Unix time subtract routine
	printf("Total time was %f seconds.\n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
  
  return 0;
}

// Computer Shubert local minimum for the given range
void shubert_local_minimum(double minx1, double maxx1, double minx2, double maxx2)
{
  double x1, x2, y = 0;
  for (x1 = minx1; x1 <= maxx1; x1 += 0.0005)
  {
    for (x2 = minx2; x2 <= maxx2; x2 += 0.0005)
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

void *child(double x1range[])
{
  double x1min = x1range[0];
  double x1max = x1range[1];
  shubert_local_minimum(x1min, x1max, -2, 2);
  // printf("Child x1 = %.1f to %.1f\n", x1min, x1max);
  pthread_exit(0);
}
