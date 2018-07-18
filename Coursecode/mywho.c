#include <fcntl.h>
#include <stdio.h>
#include <utmp.h>
#include <stdlib.h>

main(){
	struct utmp buf;
	int fd;
	if((fd=open("/var/run/utmp", O_RDONLY))==-1){
		perror("cannot open");
		exit(1);
	}
	while(read(fd, &buf, sizeof(buf))>0){
		if(buf.ut_type==USER_PROCESS){
			printf("%s\n", buf.ut_user);		
		}
	}
	close(fd);
}
