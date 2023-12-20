#include "types.h"
#include "stat.h"
#include "user.h"

//int num  = 1; //global variables for cow test
char *shared_data = (char *)0x400000;

int
main (int argc, char **argv)
{
  //pagefault();


  int pid = fork();

  if(pid < 0) {
    printf(1, "Fork failed.\n");
  }
  else if(pid == 0) {
    printf(1, "Child process is running\n");
    printf(1, "Child: Shared data before write: %s\n", shared_data);

    shared_data[0] = 'X';

    printf(1, "Child: Shared data after write: %s\n", shared_data);

    exit();
  } else {
    printf(1, "Parent process is running\n");

    sleep(100);

    printf(1, "Parent: Shared data after child write: %s\n", shared_data);

    exit();
  }
}
