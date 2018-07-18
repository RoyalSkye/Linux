#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

main(){
	int pipefd[2];
	char buf[512];
	int rv_f;
	pipe(pipefd);

	if((rv_f=fork())==-1){
		perror("cannot fork");
		exit(1);
	}

	if(0==rv_f){
		close(pipefd[1]);
		close(0);
		dup(pipefd[0]);
		close(pipefd[0]);	
		execlp("sort", "sort", NULL);
	}else{
		close(pipefd[0]);
		close(1);
		dup(pipefd[1]);
		close(pipefd[1]);
		execlp("who", "who", NULL);
	}
}
