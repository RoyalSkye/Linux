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
void client(){
	int i;
	msgqid=msgget(MSGKEY,0777|IPC_CREAT); //open 75 message queue
	//printf("msgqid:%d\n",msgqid);
	for(i=10;i>=1;i--){
		msg.mtype=i;
		strcpy(msg.mtext,"hello server");
		printf("(client)sent\n");
		msgsnd(msgqid,&msg,1024,0); //send message
	}
	exit(0);
}
main(){
	client();		
}
