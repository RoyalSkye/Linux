#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

void handle(int signum){
	printf("Ouch! The signal is %d.\n", signum);
}

main(){
	struct itimerval time;

	signal(SIGALRM, handle);
	
	time.it_value.tv_sec=2;
	time.it_value.tv_usec=50000;
	time.it_interval.tv_sec=2;
	time.it_interval.tv_usec=50000;

	setitimer(ITIMER_REAL, &time, NULL);
	
	while(1){
		printf("haha\n");
		sleep(1);
	}	
}
