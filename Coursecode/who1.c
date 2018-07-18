#include <stdio.h>
#include <fcntl.h>
#include <utmp.h>
#include <stdlib.h>
#include <time.h>

main(){
	int fd;
	struct utmp buf;
	if((fd=open("/var/run/utmp", O_RDONLY))==-1){
		perror("cannot open");
		exit(1);
	}
	while(read(fd, &buf, sizeof(buf))>0){
		if(buf.ut_type==USER_PROCESS){
			printf("%s\t%s\t%.12s\n", 
			buf.ut_user, buf.ut_line, 
			4+ctime((const time_t *)&(buf.ut_tv.tv_sec)));
		}
	}
	close(fd);
}	
