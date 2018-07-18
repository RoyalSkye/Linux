#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>

main(){
	DIR *dp;
	struct dirent *sdp;
	if((dp=opendir("."))==NULL){
		perror("cannot open");
		exit(1);
	}
	while((sdp=readdir(dp))!=NULL){
		printf("%s\n", sdp->d_name);		
	}
	closedir(dp);
}
	
