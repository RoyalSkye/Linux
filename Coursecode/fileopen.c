#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

main(){
	int fd;
	char buf[512];
	int n;
	if((fd=open("1.txt", O_RDONLY))==-1){
		perror("cannot open");
		exit(1);
	}
	
	while((n=read(fd, buf, sizeof(buf)))>0){
		printf("%.*s", n, buf);
	}

	close(fd);	
}
