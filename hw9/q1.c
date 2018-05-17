#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

// Prototypes
double shubert(double x1, double x2);

int main(int argc, char *argv[])
{
  if(argc < 2)
  {
    printf("Must specify number of threads\n");
    return 1;
  }
  omp_set_num_threads(atoi(argv[1]));

  struct timeval start_time, stop_time, elapsed_time;  // timers
	gettimeofday(&start_time,NULL); // Unix timer

  double min = 0;
  double minx1 = -2.0;
  double minx2 = -2.0;
  double maxx1 = 2.0;
  double maxx2 = 2.0;
  double x1, x2, y = 0;
  #pragma omp parallel
  {
    for (x1 = minx1; x1 <= maxx1; x1 += 0.0005)
    {
      for (x2 = minx2; x2 <= maxx2; x2 += 0.0005)
      {
        y = shubert(x1, x2);
        if (y < min)
          #pragma omp critical
          min = y;
      }
    }
    int tid = omp_get_thread_num();
    //if(tid == 0)
    //   printf("Parent x1 = -2.0 to 0.0\n");
    // else
    //   printf("Child x1 = 0.0 to 2.0\n");
  }
  printf("Global min: %.2f\n", min);

  gettimeofday(&stop_time,NULL);
	timersub(&stop_time, &start_time, &elapsed_time); // Unix time subtract routine
	printf("Total time was %f seconds.\n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
  
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
