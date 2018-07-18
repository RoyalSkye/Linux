#include <fcntl.h>
#include <stdlib.h>

main(){
	int fd;
	int s;
	if((fd=open("sync.txt", O_WRONLY|O_CREAT, 0644))==-1){
		perror("cannot open");
		exit(1);
	}
	s=fcntl(fd, F_GETFL);
	s|= O_SYNC;
	s|= O_APPEND;
	fcntl(fd, F_SETFL, s);

	write(fd, "hello file", 11);

	close(fd);
}	
