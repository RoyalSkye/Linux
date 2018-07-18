#include <stdio.h>
#include <unistd.h>

main(){
	printf("About to execute ls -l \n");
	execlp("ls", "ls", "-l", NULL);
	
	printf("done\n");
}
