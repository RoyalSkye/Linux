#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

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
		int rv_wait;
		rv_wait=wait(NULL);
		printf("done. My child %d back.\n", rv_wait);
	}
}
