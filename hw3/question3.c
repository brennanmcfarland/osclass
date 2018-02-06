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

	if(child1 == 0)
	{
		//if it's the child, fork
		printpids(2);
		child2 = fork();
		//make the child wait for its child
		if(child2 != 0)
		{
			wait(NULL);
		}
		//if it's the child of the child, just print
		else
		{
			printpids(3);
		}
	}
	//if it's the parent, wait for its child
	else
	{
		wait(NULL);
	}
}
