#include "types.h"
#include "stat.h"
#include "user.h"

int num  = 1; //global variables for cow test

int
main (int argc, char **argv)
{
  printf(1, "Free pages before fork: %d\n", getNumFreePages());

  int pid = fork();
  if(pid == 0)
  {
    printf(1, "Child: num = %d\n", num);
    printf(1, "Free pages before changing num: %d\n", getNumFreePages());
    num = 2;
    printf(1, "Child: num = %d\n", num);
    printf(1, "Free pages after changing num: %d\n", getNumFreePages());
    exit();
  }
  printf(1, "Parent: num = %d\n", num);
  wait();
  printf(1, "Parent: num = %d\n", num);
  printf(1, "Free pages after wait: %d\n", getNumFreePages());

  exit();
}
