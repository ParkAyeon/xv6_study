#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
	int k,n, id;
	double dum1 = 0, dum2 ;

	if(argc < 2)
		n = 1;
	else
		n = atoi (argv [1]);
	if(n < 0 || n > 11)
		n = 2;

	id = 0;
	for(k=0;k<n;k++)
	{
		id = fork();
		if(id<0)
			printf(1, "%d failed in fork!\n", getpid());
		else if(id > 0)
		{
			//Parent
			printf(1,"Parent %d creating chid %d\n", getpid(), id);
			wait();
		}
		else
		{
			//chlid
			printf(1, "Child %d created\n", getpid());
			for(dum2=0; dum2<50000000.0; dum2+=0.001)
				dum1 = dum1 + 30.3*30.3;
			break;
		}
	}
	exit();
}

