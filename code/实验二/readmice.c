#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

main(){
	int fd;
	int n,i;
	char buf[512];
	if((fd=open("/dev/input/mice",O_RDONLY))==-1){
		perror("cannot open:");
		exit(1);
	}
	while((n=read(fd,buf,sizeof(buf)))>0){
		for(i=0;i<n;i++){
			printf("%d",buf[i]);
		}
		fflush(stdout);
	}	
	close(fd);
}
