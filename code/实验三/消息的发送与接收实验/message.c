#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#define MSGKEY 75
struct msgform {
        long mtype;
        char mtext[1000];
}msg;
int msgqid;
main(){
	int pid;
	msgqid=msgget(MSGKEY,0777|IPC_CREAT);
	while((pid=fork())==-1);
	if(pid==0){ //child process
		msg.mtype=1;
		strcpy(msg.mtext,"I'm a child process!");
		printf("child process send\n");
		msgsnd(msgqid,&msg,1024,0);
		exit(0);
	}else{ //parent process
		wait(NULL);
		msgrcv(msgqid,&msg,1024,0,0);
		printf("parent process receive: ");
		printf("%s\n",msg.mtext);
		msgctl(msgqid,IPC_RMID,0);
		exit(0);
	}
}
