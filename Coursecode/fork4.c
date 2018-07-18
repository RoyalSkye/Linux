#include <stdio.h>

main(){
	int rv;
	rv=fork();
	if(-1==rv){
		perror("cannot fork");
		exit(1);
	}

	if(0==rv){
		printf("I am child. I cannot do what I want to do.\n");
		printf("Play!\n");
	}else{
		printf("I am parent.\n");
		printf("I am tired.\n");
	}
}
