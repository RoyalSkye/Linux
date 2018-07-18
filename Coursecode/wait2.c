#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
	int rv;
	printf("Send my son to buy sauce.\n");
	rv=fork();
	if(-1==rv){
		perror("cannot fork");
		exit(1);
	}

	if(0==rv){
		int success=0;	
		sleep(100);
		if(1==success){
			printf("Bought sauce!\n");
			return 0;
		}
		else{
			printf("failure!\n");
			return 1;
		}
	}else{
		int rv_wait;
		int status;
		rv_wait=wait(&status);
		printf("done. My child %d back.\n", rv_wait);

		if(status & 0x7F > 0){
			printf("My child die. %d killed him.\n", status & 0x7F);
			return 1;
		}
		if(status>>8 == 0){
			printf("cook\n");
		}else{
			printf("beat\n");
		}
	}
	return 0;
}
