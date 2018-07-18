#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

void modetostr(int mode, char modestr[]){
	strcpy(modestr, "----------");
	if (mode & S_IXOTH) modestr[9]='x';
	if (mode & S_IWOTH) modestr[8]='w';
	if (mode & S_IROTH) modestr[7]='r';
	if (mode & S_IXGRP) modestr[6]='x';
	if (mode & S_IWGRP) modestr[5]='w';
	if (mode & S_IRGRP) modestr[4]='r';
	if (mode & S_IXUSR) modestr[3]='x';
	if (mode & S_IWUSR) modestr[2]='w';
	if (mode & S_IRUSR) modestr[1]='r';
	if (S_ISDIR(mode)) modestr[0]='d';
}

main(){
	DIR *dp;
	struct dirent *sdp;
	struct stat buf;
	char modestr[11];
	if((dp=opendir("."))==NULL){
		perror("cannot open");
		exit(1);
	}
	while((sdp=readdir(dp))!=NULL){
		if(sdp->d_name[0]!='.'){
			stat(sdp->d_name, &buf);
			modetostr(buf.st_mode, modestr);
			printf("%s\t", modestr);
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

