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
		if(1==success){
			printf("Bought sauce!\n");
			return 0;
		}
		else{
			printf("failure!\n");
			return 1;
		}
	}else{
		sleep(100);
	}
	return 0;
}
