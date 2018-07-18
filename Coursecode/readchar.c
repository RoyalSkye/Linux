#include <stdio.h>

main(){
	char c;
	while((c=getchar())!=EOF){
		printf("%d:%c", c, c);
		fflush(stdout);
	}
}
		
