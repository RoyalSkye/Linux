#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include<string.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void modetrans(mode_t mode,char *S){
	strcpy(S,"----------.");
	if(S_ISDIR(mode)) *S='d';

	if(mode & S_IRUSR) *(S+1)='r';
	if(mode & S_IWUSR) *(S+2)='w';
	if(mode & S_IXUSR) *(S+3)='x';
	
	if(mode & S_IRGRP) *(S+4)='r';
        if(mode & S_IWGRP) *(S+5)='w';
        if(mode & S_IXGRP) *(S+6)='x';
	
	if(mode & S_IROTH) *(S+7)='r';
        if(mode & S_IWOTH) *(S+8)='w';
        if(mode & S_IXOTH) *(S+9)='x';
}
main(int ac,char* av[]){
	DIR *dp;
	int a;
        struct dirent *p;
        struct stat buf;
        char modestr[11];
	if(ac==1){
		if ((dp=opendir("."))==NULL){
                	perror("cannot open");
               		 exit(1);	
       		 }
	}else if(ac==2){
		if((a=chdir(av[1]))==-1){
			perror("cannot change directory");
			exit(1);
		}else{
			 if ((dp=opendir("."))==NULL){
                        	perror("cannot open");
                         	exit(1);
			}
		}
	}else{
		perror("error input!");
		exit(1);
	}

	while((p=readdir(dp))!=NULL){
		if(p->d_name[0]!='.'){
			stat(p->d_name,&buf);
			modetrans(buf.st_mode,modestr);
			printf("%s ",modestr);
			printf("%d ",buf.st_nlink);
			printf("%s ",getpwuid(buf.st_uid)->pw_name);
			printf("%s ",getgrgid(buf.st_gid)->gr_name);
			printf("%6d ",buf.st_size);
			printf("%-20s ",p->d_name);
			printf("%-20s",4+ctime((const time_t *)&(buf.st_mtime)));
		}
	}
	closedir(dp);
}

