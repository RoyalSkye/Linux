#include <fcntl.h>
#include <stdlib.h>

main(){
	int fd;
	int s;
	if((fd=open("sync.txt", O_WRONLY|O_CREAT|O_SYNC|O_APPEND, 0644))==-1){
		perror("cannot open");
		exit(1);
	}
	write(fd, "haha file", 11);

	close(fd);
}	
