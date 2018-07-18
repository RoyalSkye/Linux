#include <stdio.h>
#include <signal.h>

main(){
	signal(2, SIG_IGN);
	
	while(1){
		printf("You cannot stop me!\n");
	}
}
