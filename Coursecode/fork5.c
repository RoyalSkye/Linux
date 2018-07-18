#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

main(){
	int rv;
	printf("About to execute ls\n");
	rv=fork();
	if(-1==rv){
		perror("cannot fork");
		exit(1);
	}

	if(0==rv){
		execlp("ls", "ls", NULL);
	}else{
		printf("done\n");
	}
}
