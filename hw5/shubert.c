// Chris Fietkiewicz. Searches for a minimum in Shubert function.
// Note that an interval of 0.5 gives a very inaccurate result but
// works as a fast demonstration. For a realistic search, we would
// use a much small interval and remove the printing from the loop.
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define READ_END 0
#define WRITE_END 1

// Prototypes
double shubert(double x1, double x2);
double shubert_local_minimum(double minx1, double maxx1, double minx2, double maxx2);
double write_to_pipe(int fd[2], void *value, int size);
void *read_from_pipe(int fd[2], int size);

int main ()
{
  double min = 0;

  /*fork two processes and have them find the minima in each half of the space*/
  pid_t pid1, pid2 = 0;
  int fd1[2], fd2[2];
  if(pipe(fd1) == -1 || pipe(fd2) == -1)
  {
    fprintf(stderr, "Pipe failed");
    return 1;
  }

  pid1 = fork();
  if(pid1 != 0) { pid2 = fork(); }
  //first child calculates local minimum and writes it to the pipe
  if(pid1 == 0)
  {
    double lmin = shubert_local_minimum(-2, 0, -2, 2);
    write_to_pipe(fd1, (void *)&lmin, sizeof(double));
  }
  //second child calculates local minimum and writes it to the pipe
  else if(pid2 == 0)
  {
    double lmin = shubert_local_minimum(0, 2, -2, 2);
    write_to_pipe(fd2, (void *)&lmin, sizeof(double));
  }
  //the parent gets the local minima from pipes to calculate global minimum
  else
  {
    wait(NULL);
    wait(NULL);
    //TODO: segfaulting here
    void *test = read_from_pipe(fd1, 8);
    double lmin1 = *(double *)read_from_pipe(fd1, sizeof(double));
    double lmin2 = *(double *)read_from_pipe(fd2, sizeof(double));
    printf("minimum = %.2f \n", fmin(lmin1, lmin2));
  }
}

// Computer Shubert local minimum for the given range
double shubert_local_minimum(double minx1, double maxx1, double minx2, double maxx2)
{
  double x1, x2, min, y = 0;
  for (x1 = minx1; x1 <= maxx1; x1 += 0.5)
  {
    for (x2 = minx2; x2 <= maxx2; x2 += 0.5)
    {
      y = shubert(x1, x2);
      printf("%.2f ", y);
      if (y < min)
        min = y;
    }
    printf("\n", y);
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
