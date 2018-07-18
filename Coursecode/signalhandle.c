#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handle(int signum){
	printf("Ouch! The signal is %d.\n", signum);
}

main(){
	signal(2, handle);
	signal(3, handle);
	
	while(1){
		printf("haha\n");
		sleep(1);
	}
}
