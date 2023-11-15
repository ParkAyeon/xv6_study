#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {

  char *args[] = {"echo", "NEWPROC_CALL", 0};
	if(fork()==0){
		printf(1,"complete fork\n");
	 	exec(args[0], args);
    exit();
	}
	 else{
	 	printf(1,"parent process\n");
    exit();
	}
}
