#include <stdio.h>  
#include <sys/types.h> 
#include <unistd.h>  
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>

int main (int argc, char** argv)
{
	pid_t pid1;
	pid_t pid2;	
	int sacha = 0;
	int regis = 0;
	int capture = 0;
	

	int which = PRIO_PROCESS;
	

	if ((pid1 = fork())==0)
	{
		int i=0;
		if ( argc >= 3 ){
		int prio = atoi(argv[1]);
		nice(prio);
        }
		for (sacha ; sacha < 151 ; sacha++)
		{	
			printf("Sacha : %d\n", sacha);
			fflush(stdout);
			for (i;i<2000000;i++){};
		} 

		printf("Sacha a capturé les 151 pokémons !\n");
		fflush(stdout);
	}
	else if ((pid2 = fork())==0)
	{
		int i=0;
		if ( argc >= 3 ){
		int prio = atoi(argv[2]);
		nice(prio);
        }
		for (regis ; regis < 151 ; regis++)
		{
			printf("Regis : %d\n", regis);
			fflush(stdout);
			for (i;i<2000000;i++){};
		} 

		printf("Régis a capturé les 151 pokémons !\n");
		fflush(stdout);
	}
	else
	{
		while (waitpid(pid1,NULL,0)!=-1);
		while (waitpid(pid2,NULL,0)!=-1);
		exit(0);
	}


}
