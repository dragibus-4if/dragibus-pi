#include <stdio.h>  
#include <sys/types.h> 
#include <unistd.h>  
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>

int main (int argc, char** argv)
{	
	int sacha = 0;
	
		char* string = argv[1];
		for (sacha ; sacha < 151 ; sacha++)
		{	
			printf(string);
			printf(" : %d\n", sacha);
			fflush(stdout);
			for (int i=0;i<2000000;i++){};
		} 

}
