#include <stdio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#define N 1000000000

int main() {
  int num_processes;
	long int i;
	long long int x = 0;
	pid_t pid;
	struct timeval start_time, stop_time, elapsed_time;  // timers

  printf("How many processes? ");
  scanf("%d", &num_processes);

  gettimeofday(&start_time,NULL); // Unix timer
  if(num_processes > 1)
	  pid = fork();
	if (pid > 0 && num_processes > 2) {
		pid = fork();
		if (pid > 0 && num_processes > 3) {
			pid = fork();
		}
	}
	for (i = 0; i < N/num_processes; i++) {
		x = x + 1;
	}
	gettimeofday(&stop_time,NULL);
	timersub(&stop_time, &start_time, &elapsed_time); // Unix time subtract routine
	printf("Process %d total time was %f seconds. x = %lld.\n", getpid(), elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0, x);
}
