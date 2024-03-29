#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <math.h>
#include <sys/time.h>


const int SHARED_MEMORY_SIZE = sizeof(double);
const int SHARED_MEMORY_ADDRESS = 0432;
const char *SHARED_MEMORY_NAME = "os_shared_minimum";
const int SHARED_SEMAPHORE_SIZE =  sizeof(sem_t);
const int SHARED_SEMAPHORE_ADDRESS = 1407;
const char *SHARED_SEMAPHORE_NAME = "os_shared_semaphore";
int shm_fd;
int sem_fd;
int pid = 0;
void *ptr;
void *sem;

double shubert(double x1, double x2);

int main(int argc, char *argv[])
{
	if(argc < 2)
  {
    printf("Must specify number of threads\n");
    return 1;
  }

	struct timeval start_time, stop_time, elapsed_time;  // timers
	gettimeofday(&start_time,NULL); // Unix timer

  int num_threads = atoi(argv[1]);

  //initialize shared memory
	shm_fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, SHARED_MEMORY_ADDRESS);
	ftruncate(shm_fd,SHARED_MEMORY_SIZE);
	ptr = mmap(0,SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
		return -1;
	}
  sem_fd = shm_open(SHARED_SEMAPHORE_NAME, O_CREAT | O_RDWR, SHARED_SEMAPHORE_ADDRESS);
	ftruncate(sem_fd,SHARED_SEMAPHORE_SIZE);
	sem = mmap(0,SHARED_SEMAPHORE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sem_fd, 0);
	if (sem == MAP_FAILED) {
		printf("Map failed\n");
		return -1;
  }

	//zero out the shared memory
	memset(ptr, 0, SHARED_MEMORY_SIZE);

  //initialize the semaphore
  if(sem_init((sem_t *)sem, 0, 1) < 0)
  {
    fprintf(stderr, "Unable to initialize semaphore\n");
    exit(1);
  }

  double x1globalmin = -2.0;
  double x1globalmax = 2.0;
  double x2min = -2.0;
  double x2max = 2.0;
  double x1, x2, y = 0;

	//set initial x1 values
	double stepsize = (x1globalmax-x1globalmin)/num_threads;
	double x1min = x1globalmin;
	double x1max = x1min + stepsize;
	pid = fork();
	//and increment with each fork
	int threadnum;

	for(threadnum = 1; threadnum < num_threads; threadnum++)
	{
		if(pid != 0)
		{
			x1min += stepsize;
			x1max += stepsize;
			pid = fork();
			if(pid != 0 && threadnum == num_threads)
			{
				x1min += stepsize;
				x1max += stepsize;
			}
		}
	}

  //calculate the local minimum for the given range for each process
	for (x1 = x1min; x1 <= x1max; x1 += .005)
	{
	  for (x2 = x2min; x2 <= x2max; x2 += .00005)
	  {
	    y = shubert(x1, x2);
	    if(y < *(double *)ptr)
	    {
	      sem_wait((sem_t *)sem); //wait for semaphore availability
	      memcpy(ptr, &y, sizeof(double));
	      sem_post((sem_t *)sem); //post that the semaphore is available
	    }
	  }
	}

  if(pid != 0)
  {
		for(threadnum = 0; threadnum < num_threads-1; threadnum++)
		{
			wait(NULL);
		}
    printf("Global min: %.2f\n", *(double *)ptr);
    sem_destroy((sem_t *)sem);//destroy semaphore
		//destroy shared memory
  	if (shm_unlink(SHARED_MEMORY_NAME) == -1) {
  		printf("Error removing %s\n",SHARED_MEMORY_NAME);
  		exit(-1);
  	}
		if(shm_unlink(SHARED_SEMAPHORE_NAME) == -1) {
			printf("Error removing %s\n",SHARED_SEMAPHORE_NAME);
  		exit(-1);
		}

		gettimeofday(&stop_time,NULL);
	timersub(&stop_time, &start_time, &elapsed_time); // Unix time subtract routine
	printf("Total time was %f seconds.\n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
  }
	else
	{
		// printf("Child x1 = %.1f to %.1f\n", x1min, x1max);
	}


  return 0;
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
