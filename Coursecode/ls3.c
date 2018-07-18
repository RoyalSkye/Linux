#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

main(){
	DIR *dp;
	struct dirent *sdp;
	struct stat buf;
	if((dp=opendir("."))==NULL){
		perror("cannot open");
		exit(1);
	}
	while((sdp=readdir(dp))!=NULL){
		if(sdp->d_name[0]!='.'){
			stat(sdp->d_name, &buf);
			if(buf.st_mode & 4)
				printf("o_r\t");
			printf("%d\t", buf.st_mode);
			printf("%d\t", buf.st_nlink);
			printf("%d\t", buf.st_uid);
			printf("%d\t", buf.st_gid);
			printf("%d\t", buf.st_size);
			printf("%d\t", buf.st_mtime);	
			printf("%s\n", sdp->d_name);		
		}
	}
	closedir(dp);
}

