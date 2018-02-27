#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>

// Prototypes
double shubert(double x1, double x2);
double shubert_local_minimum(double minx1, double maxx1, double minx2, double maxx2);
void *child(); //thread

double lmin1, lmin2;

int main()
{
  pthread_t tid;
  pthread_attr_t attr;

  //get default attributes
  pthread_attr_init(&attr);

  //create child thread, which will calculate the local min for one half
  pthread_create(&tid, &attr, child, NULL);

  //meanwhile, calculate the local min for the other half
  lmin1 = shubert_local_minimum(-2, 0, -2, 2);

  //wait for the child thread to exit and get its local min
  pthread_join(tid, NULL);
  printf("\nLocal min 1: %.2f\n", lmin1);
  printf("\nLocal min 2: %.2f\n", lmin2);
  double gmin = fmin(lmin1, lmin2);
  printf("\nGlobal min: %.2f\n", gmin);

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

void *child()
{
  lmin2 = shubert_local_minimum(0, 2, -2, 2);
  pthread_exit(0);
}
