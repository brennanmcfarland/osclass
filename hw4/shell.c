#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 80 // max line length
#define PROMPT_TEXT "simpleshell"
#define TRUE 1
#define FALSE 0

char *args[MAX_LINE/2 + 1]; // command line arguments
char line[MAX_LINE];

//returns if this is the parent process
int execute_command()
{
  int pid = 0;
  pid = fork();
  if(pid == 0)
  {
    execvp(args[0], args);
    return TRUE;
  }
  else
  {
    wait(NULL);
    return FALSE;
  }
}

void get_input()
{
  fgets(line, MAX_LINE, stdin);
  line[strlen(line)-1] = '\0';    //truncate the newline at the end
  args[0] = strtok(line, " ");
  int i = 0;
  while (args[i] != NULL)
    args[++i] = strtok(NULL, " ");
}

int main(void)
{
  while (TRUE) {
    fflush(stdin);
    printf("%s>", PROMPT_TEXT);
    fflush(stdout);
    fgets(line, MAX_LINE, stdin);
    line[strlen(line)-1] = '\0';    //truncate the newline at the end
    args[0] = strtok(line, " ");
    int i = 0;
    while (args[i] != NULL)
      args[++i] = strtok(NULL, " ");
    if(strcmp(args[0], "exit") == 0) { return 0; }
    if(execute_command()) { return 0; }
  }
}
