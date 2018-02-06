#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

void printpids(short procnum)
{
	printf("Process %u = %u, parent = %u\n", procnum, getpid(), getppid());
	fflush(stdout);
}

void printchildexit(pid_t childpid)
{
	printf("Child %u exited.\n", childpid);
}

int main()
{
	pid_t child1 = 0;
	pid_t child2 = 0;

	printpids(1);
	child1 = fork();
	//if we're the parent, fork again
	if(child1 != 0)
	{
		child2 = fork();
	}

	//if it's the second child
	if(child1 != 0 && child2 == 0)
	{
		sleep(1);
		printpids(3);
		sleep(3);
	}
	//if it's the first child
	else if(child1 == 0 && child2 == 0)
	{
		sleep(.5);
		printpids(2);
		sleep(2);
	}
	//if it's the parent
	else
	{
		wait(NULL); //wait for the first child to terminate
		printchildexit(child1);
		wait(NULL); //wait for the second child to terminate
		printchildexit(child2);
	}
}
