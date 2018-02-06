#include <unistd.h>
#include <stdio.h>


int main()
{
	pid_t pid = fork();
	if(pid > 0)
		printf("Line 3");
	else
		printf("Line 5");
}
