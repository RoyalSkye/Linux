#include <stdio.h>

main(){
	fork();
	fork();
	fork();
	printf("haha. I am %d\n", getpid());
}
