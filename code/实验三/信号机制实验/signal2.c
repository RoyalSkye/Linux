#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int pid1,pid2;
void IntDelete(){
	kill(pid1,10);
	kill(pid2,12);
}
void Int1(){
	printf("child process 1 is killed by parent!\n");
	exit(0);	
}
void Int2(){
        printf("child process 2 is killed by parent!\n");
        exit(0);        
}
main(){
	signal(SIGINT,SIG_IGN);//if there is ctrl+c,do not terminate
	signal(SIGQUIT,SIG_IGN);
	while((pid1=fork())==-1);
	if(pid1==0){  //child1 process
		signal(SIGUSR1,Int1);
		signal(SIGINT,SIG_IGN);
		pause();
		exit(0);
	}else{  //parent process
		while((pid2=fork())==-1);
		if(pid2==0){  //child2 process
			signal(SIGUSR2,Int2);
			signal(SIGINT,SIG_IGN);
			pause();
			exit(0);
		}else{  //parent process
			signal(SIGINT,IntDelete);
			waitpid(-1,NULL,0); //wait for any child process terminate,eq wait()
			waitpid(-1,NULL,0);
			printf("parent process is killed \n");
			exit(0);
		}
	}	
}
