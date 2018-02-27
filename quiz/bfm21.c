/*
  Brennan McFarland
  bfm21
*/
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


const int SHARED_MEMORY_SIZE = 4096;
const int SHARED_MEMORY_ADDRESS = 0666;
const char *SHARED_MEMORY_NAME = "os_shared_memory";
int shm_fd;
void *ptr;

void run_top()
{
  char *args[4];
  args[0] = "top";
  args[1] = "-u";
  args[2] = "bfm21";
  args[3] = 0;
  execvp(args[0], args);
}

int main(void) {

  int pid = 0;
  pid = fork();
  //run top as the first child
  if(pid == 0)
  {
    run_top();
  }
  else
  {
    shm_fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, SHARED_MEMORY_ADDRESS);
    ftruncate(shm_fd,SHARED_MEMORY_SIZE);
    ptr = mmap(0,SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    pid = fork();
    //increment nc times as the second child
    if(pid == 0)
    {
      int nc = 100000000;
      int sum = 0;
      for(int i=0; i<nc; i++){
        sum++;
      }
      //put in shared memory
      sprintf(ptr, "%d", sum);
    }
    //parent
    else
    {
      int np = 200000000;
      int sum = 0;
      for(int i=0; i<np; i++){
        sum++;
      }
      wait(NULL);
      wait(NULL);
      printf("sum = %d", sum+atoi(ptr));
      if (shm_unlink(SHARED_MEMORY_NAME) == -1) {
    		printf("Error removing %s\n",SHARED_MEMORY_NAME);
    		exit(-1);
    	}
    }
  }
 return 0;
}
