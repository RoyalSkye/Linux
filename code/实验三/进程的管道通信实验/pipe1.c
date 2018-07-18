#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int pid1,pid2;
main(){
	int fd[2];
	char OutPipe[100],InPipe[100];
	pipe(fd);  //fd[0]:r fd[1]:w
	while((pid1=fork())==-1);
	if(pid1==0){ //child1 process
		lockf(fd[1],1,0);
		sprintf(OutPipe,"child 1 process is sending message!");
		write(fd[1],OutPipe,50);
		sleep(2);
		lockf(fd[1],0,0);
		exit(0);
	}else{	//parent proccess
		while ((pid2=fork())==-1);
		if(pid2==0){ //child2 process
			lockf(fd[1],1,0);
			sprintf(OutPipe,"child 2 process is sending message!");
			write(fd[1],OutPipe,50);
			sleep(2);
			lockf(fd[1],0,0);
			exit(0);
		}else{ //parent process
			wait(0);
			read(fd[0],InPipe,200);
			printf("%s\n",InPipe);
			wait(0);
			read(fd[0],InPipe,50);
			printf("%s\n",InPipe);
			exit(0);
		}
	}
}
