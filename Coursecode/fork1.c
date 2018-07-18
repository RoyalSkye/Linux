#include <stdio.h>

main(){
	printf("I am parent. I am %d.\n", getpid());
	
	fork();

	printf("I am %d.\n", getpid());
}
