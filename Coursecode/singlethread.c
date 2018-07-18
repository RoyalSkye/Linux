#include <stdio.h>

void print(char *s){
	int i;
	for(i=0;i<5;i++){
		printf("%s\n", s);
		sleep(1);
	}
}

main(){
	print("hello");
	print("world");
}
