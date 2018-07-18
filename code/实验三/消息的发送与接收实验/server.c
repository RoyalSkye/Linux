#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#define MSGKEY 75
struct msgform {
	long mtype;
	char mtext[1000];
}msg;
int msgqid;
void server(){
	msgqid=msgget(MSGKEY,0777|IPC_CREAT); //open or create 75 message queue
	//printf("msgqid=%d\n",msgqid);
	do{
		msgrcv(msgqid,&msg,1024,0,0); //receive message
		printf("(server)received: ");
	   	printf("%s\n",msg.mtext);
	}while(msg.mtype!=1);
	msgctl(msgqid,IPC_RMID,0); //delete message queue,return resources
	exit(0);
}
main(){
	server();		
}
