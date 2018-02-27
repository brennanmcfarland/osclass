#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define BUFFER_SIZE sizeof(double)
#define READ_END 0
#define WRITE_END 1

// Prototypes
double shubert(double x1, double x2);
double shubert_local_minimum(double minx1, double maxx1, double minx2, double maxx2);
double write_to_pipe(int fd[2], void *value, int size);
void *read_from_pipe(int fd[2], int size);

/* the two child processes calculate the local minimum of half the space each,
then pass that back to the parent, which determines which is the global min
*/
int main()
{
  double write_buffer[BUFFER_SIZE];
  pid_t pid1, pid2 = 0;
  int fd1[2], fd2[2];

  if(pipe(fd1) == -1 || pipe(fd2) == -1)
  {
    fprintf(stderr, "Failed to create pipe\n");
    return 1;
  }

  pid1 = attempt_fork();

  //child 1
  if(pid1 == 0)
  {
    double double_to_write = shubert_local_minimum(-2, 0, -2, 2);
    memcpy(write_buffer, &double_to_write, BUFFER_SIZE);
    write_to_pipe(fd1, write_buffer, strlen(write_buffer)+1);
  }
  else
  {
    pid2 = attempt_fork();
    //child 2
    if(pid2 == 0)
    {
      double double_to_write = shubert_local_minimum(0, 2, -2, 2);
      memcpy(write_buffer, &double_to_write, BUFFER_SIZE);
      write_to_pipe(fd2, write_buffer, strlen(write_buffer)+1);
    }
    //parent
    else
    {
      wait(NULL);
      wait(NULL);
      double lmin1, lmin2;
      memcpy(&lmin1, read_from_pipe(fd1, BUFFER_SIZE), BUFFER_SIZE);
      printf("\nLocal min 1: %.2f\n", lmin1);
      memcpy(&lmin2, read_from_pipe(fd2, BUFFER_SIZE), BUFFER_SIZE);
      printf("\nLocal min 2: %.2f\n", lmin2);
      double gmin = fmin(lmin1, lmin2);
      printf("\nGlobal min: %.2f\n", gmin);
    }
  }
  return 0;
}

// Computer Shubert local minimum for the given range
double shubert_local_minimum(double minx1, double maxx1, double minx2, double maxx2)
{
  double x1, x2, min, y = 0;
  for (x1 = minx1; x1 <= maxx1; x1 += 0.01)
  {
    for (x2 = minx2; x2 <= maxx2; x2 += 0.01)
    {
      y = shubert(x1, x2);
      if (y < min)
        min = y;
    }
  }
  return min;
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

double write_to_pipe(int fd[2], void *value, int size)
{
  close(fd[READ_END]);
  write(fd[WRITE_END], value, size);
  close(fd[WRITE_END]);
}

void *read_from_pipe(int fd[2], int size)
{
  char *value = malloc(size);
  close(fd[WRITE_END]);
  read(fd[READ_END], value, size);
  close(fd[READ_END]);
  //why is value 0 here?
  return value;
}

pid_t attempt_fork()
{
  pid_t pid;
  if((pid = fork()) < 0)
  {
    fprintf(stderr, "Failed to fork\n");
    exit(1);
  }
  return pid;
}
