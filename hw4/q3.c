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

int main()
{
  const int SHARED_MEMORY_SIZE = 4096;
  const int SHARED_MEMORY_ADDRESS = 0666;
  const char *SHARED_MEMORY_NAME = "os_shared_memory";
  const char *message = "Hello from your child!";
  int shm_fd;
  int pid = 0;
	void *ptr;

	/* create the shared memory segment */
	shm_fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, SHARED_MEMORY_ADDRESS);

	/* configure the size of the shared memory segment */
	ftruncate(shm_fd,SHARED_MEMORY_SIZE);

	/* now map the shared memory segment in the address space of the process */
	ptr = mmap(0,SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
		return -1;
	}

  /* create the child process */
  pid = fork();

  /* if it's the child, print to the shared memory */
  if(pid == 0)
  {
    sprintf(ptr, "%s", message);
  }
  /* if the parent, read and print what's in the shared memory and unlink */
  else
  {
    /* wait for the child to finish printing the message before displaying it */
    wait(NULL);
    printf("%s", ptr);
    /* remove the shared memory segment */
  	if (shm_unlink(SHARED_MEMORY_NAME) == -1) {
  		printf("Error removing %s\n",SHARED_MEMORY_NAME);
  		exit(-1);
  	}
  }
}
