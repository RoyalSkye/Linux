#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

main(){
	/*getpid() returns the process ID of the calling process.
	getppid() returns the process ID of the parent of the calling  process
	Question: at first,the return value of getppid() is 1.Because the parents process quit
	earlier than the child process,and the child process is managed by init whose
	pid_t is 1.TO solve this problem,we add sleep(1) to every parents process.*/
	pid_t p1,p2,p3;
	printf("this is parents,currentpid:%d parentspid:%d\n",getpid(),getppid());
	while((p1=fork())==-1);
	//printf("current:%d,p1=%d\n",getpid(),p1);
	if(p1==0){
		printf("this is p1,currentpid:%d parentspid:%d\n",getpid(),getppid());
		while((p2=fork())==-1);
		//printf("current:%d,p2=%d\n",getpid(),p2);
		if(p2==0){
			printf("this is p2,currentpid:%d parentspid:%d\n",getpid(),getppid());
			while((p3=fork())==-1);
			//printf("current:%d,p3=%d\n",getpid(),p3);
			if(p3==0){ 
				printf("this is p3,currentpid:%d parentspid:%d\n",getpid(),getppid());
			}
			sleep(1);
		}
		sleep(1);
	}
	sleep(1);
}
