#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
 
int main ()
{
  pid_t child_pid;
  int child_status;
 
  child_pid = fork ();
  if (child_pid > 0) {
    printf("\nparent process - %d\n", getpid());    
    sleep(20); 
    wait(NULL);
    printf("\n\twait\n");
    sleep(10);
    printf("\n\tend of parent working\n");
    exit(0);
  }
  else if (child_pid == 0) {
    printf("child process - %d\n", getpid());
    sleep (10);
    printf("\n\texit child process\n");
    exit(0);    
  }
  
  return 0;
}