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


const int SHARED_MEMORY_SIZE = sizeof(double);
const int SHARED_MEMORY_ADDRESS = 0332;
const char *SHARED_MEMORY_NAME = "os_shared_minimum";
const int SHARED_SEMAPHORE_SIZE =  sizeof(sem_t);
const int SHARED_SEMAPHORE_ADDRESS = 1307;
const char *SHARED_SEMAPHORE_NAME = "os_shared_semaphore";
int shm_fd;
int sem_fd;
int pid = 0;
void *ptr;
void *sem;

double shubert(double x1, double x2);

int main()
{
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

  double x1min = -2.0;
  double x1max = 0.0;
  double x2min = -2.0;
  double x2max = 2.0;
  double x1, x2, y = 0;
  //fork the child and set the range of the shubert function
  pid = fork();
  if(pid == 0)
  {
    x1min = 0.0;
    x1max = 2.0;
  }

  //calculate the local minimum for the given range for each process
  for (x1 = x1min; x1 <= x1max; x1 += .001)
  {
    for (x2 = x2min; x2 <= x2max; x2 += .001)
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
		printf("Parent x1 = %.1f to %.1f\n", x1min, x1max);
    wait(NULL);
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
  }
	else
	{
		printf("Child x1 = %.1f to %.1f\n", x1min, x1max);
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
