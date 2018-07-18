#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

main(int ac,char *av[]){
	int fd1,fd2,n;
	char buf[512];
	//printf("ac=%d\n",ac);
	if(ac!=3){
		 printf("error input!\n");
		exit(1);
	}	
	if((fd1=open(av[1],O_RDONLY))==-1){
		perror("cannot open");
		exit(1);
	}
	if((fd2=open(av[2],O_WRONLY|O_CREAT,0644))==-1){
		perror("error");
		exit(1);
	}else{
		while((n=read(fd1,buf,sizeof(buf)))>0){
			write(fd2,buf,n);
		}
	}
	close(fd1);
	close(fd2);
}
