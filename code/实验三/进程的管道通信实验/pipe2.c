#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
//not clear???
int pid1,pid2;
main(){
	int fd[2];
	char OutPipe[100],InPipe[100];
	pipe(fd);  //fd[0]:r fd[1]:w
	while((pid1=fork())==-1);
	if(pid1==0){ //child1 process
		close(fd[1]);
		lockf(fd[0],1,0);
		read(fd[0],OutPipe,12);
		printf("%s",OutPipe);
		lockf(fd[0],0,0);
		exit(0);
	}else{	//parent proccess
		while ((pid2=fork())==-1);
		if(pid2==0){ //child2 process
			close(fd[1]);
                	lockf(fd[0],1,0);
                	read(fd[0],OutPipe,12);
                	printf("%s",OutPipe);
                	lockf(fd[0],0,0);
               		exit(0);
		}else{ //parent process
			close(fd[0]);
			sprintf(InPipe,"this is pipe2.c test!");
			write(fd[1],InPipe,100);
			wait(0);
			wait(0);
			exit(0);
		}
	}
}
