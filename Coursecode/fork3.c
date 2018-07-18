#include <stdio.h>

main(){
	int rv;
	printf("I am parent. I am %d.\n", getpid());
	
	rv=fork();

	if(-1==rv){
		perror("cannot fork");
		exit(1);
	}

	printf("I am %d. The fork() returned %d.\n", getpid(), rv);
}
