#include <stdio.h>

main(){
	int p1,p2;
	while((p1=fork())==-1);
	if(p1==0) putchar('b');  //in child1 process
	else{
		while((p2=fork())==-1);
		if(p2==0) putchar('c'); //in child2 process
		else putchar('a');  //in parent process
	}
}
