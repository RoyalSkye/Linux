#include <sys/types.h> 
#include <sys/socket.h> 
#include <stdio.h> 
#include <netinet/in.h>                                                                    
#include <arpa/inet.h> 
#include <unistd.h> 
#include <time.h>
#include <strings.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <aio.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <signal.h>

#define BUFF_SIZE  1024   

char buf[BUFF_SIZE];
char identify;
int id;
int flag=1;

void aioread1(int fd){  //for user error input
	int ret;
	struct aiocb my_aiocb;
	memset(buf, 0, sizeof(buf));
 	bzero((char *)&my_aiocb,sizeof(struct aiocb));
 	my_aiocb.aio_buf = malloc(BUFF_SIZE+1);
  	if(!my_aiocb.aio_buf) perror("malloc");
 	my_aiocb.aio_fildes = fd;
  	my_aiocb.aio_nbytes = BUFF_SIZE;
  	my_aiocb.aio_offset = 0;
  	ret = aio_read(&my_aiocb);
  	if (ret < 0) perror("aio_read1");
 	while (aio_error(&my_aiocb) == EINPROGRESS); 
  	if((ret = aio_return(&my_aiocb))>0){
		printf("无效的输入:%s\n",my_aiocb.aio_buf);
  	}
}
void aioread(int fd){
	int ret;
	struct aiocb my_aiocb;
	memset(buf, 0, sizeof(buf));
 	bzero((char *)&my_aiocb,sizeof(struct aiocb));
 	my_aiocb.aio_buf = malloc(BUFF_SIZE+1);
  	if(!my_aiocb.aio_buf) perror("malloc");
 	my_aiocb.aio_fildes = fd;
  	my_aiocb.aio_nbytes = BUFF_SIZE;
  	my_aiocb.aio_offset = 0;
  	ret = aio_read(&my_aiocb);
  	if (ret < 0) perror("aio_read");
 	while (aio_error(&my_aiocb) == EINPROGRESS); 
  	if((ret = aio_return(&my_aiocb))>0){
		memset(buf, 0, sizeof(buf));
		strcpy(buf,(const char *)my_aiocb.aio_buf);
		//printf("ret:%d\n",ret);
		printf("receive message from server:%s\n",my_aiocb.aio_buf);
  	}else{
  		printf("您已阵亡!服务器已断开连接!\n");
		close(fd);
		exit(1);
	}
}

void aiowrite(int fd,char *buf){
	struct aiocb my_aiocb;
    	int ret;
	bzero((char *)&my_aiocb,sizeof(struct aiocb));
	my_aiocb.aio_buf = malloc(BUFF_SIZE+1);
  	if(!my_aiocb.aio_buf) perror("malloc");
 	my_aiocb.aio_fildes = fd;
  	my_aiocb.aio_nbytes = BUFF_SIZE;
  	my_aiocb.aio_offset = 0;
	my_aiocb.aio_buf =buf;
	ret = aio_write(&my_aiocb);
    	if(ret < 0) perror("aio_write");
	while(aio_error(&my_aiocb) == EINPROGRESS) ;
	if((ret = aio_return(&my_aiocb))>0){
		//printf("ret:%d\n",ret);
		printf("send message to server:%s\n",my_aiocb.aio_buf);
  	}else{
  		printf("send error!\n");
	}
}

void clear(int sock,char *buf1){
	int res,i;
	fd_set readfds;
	fd_set readfds_bak;
	struct timeval timeout;
	FD_ZERO(&readfds);
	FD_ZERO(&readfds_bak);
	FD_SET(0,&readfds_bak);
	FD_SET(sock,&readfds_bak);
	while(1){  
		timeout.tv_sec = 10;
	      timeout.tv_usec = 0;
		readfds = readfds_bak;
		res=select(sock+1, &readfds, NULL, NULL, &timeout);
		if(res>0){
			if(FD_ISSET(0,&readfds)){ 
				aioread1(0);  //error input
			}
			if(FD_ISSET(sock,&readfds)){  //from server game begin
				aioread(sock);
				if(!strcmp(buf,buf1)){
					break;
				}
			}
		}else if(res==0)	
			printf("waiting for server!\n");
		else{
			perror("select failed");
			exit(1);
		}
	}
}

void game(int sock){
	printf("游戏开始！\n");
	if(identify=='A'){
		printf("您的身份为平民,游戏ID为%d\n",id);
	}else if(identify=='B'){
		printf("您的身份为狼人,游戏ID为%d\n",id);
	}else if(identify=='C'){
		printf("您的身份为女巫,游戏ID为%d\n",id);
	}else if(identify=='D'){
		printf("您的身份为预言家,游戏ID为%d\n",id);
	}else{
		printf("error!\n");
		return;
	}
	while(flag){
		char c;
		aioread(sock);
		aioread(sock);
		if(identify=='B'){
			memset(buf, 0, sizeof(buf));
			while((c=getchar())=='\n');
			buf[0]=c;
			//buf[0]=getchar();
			printf("buf:%s\n",buf);
			aiowrite(sock,buf);
		}
		memset(buf, 0, sizeof(buf));
		strcpy(buf, "wolfover");
		clear(sock,buf);
		if(identify=='C'){
			aioread(sock);
			memset(buf, 0, sizeof(buf));
			while((c=getchar())=='\n');
			buf[0]=c;
			//buf[0]=getchar();
			buf[1]=getchar();
			buf[2]=getchar();
			printf("buf:%s\n",buf);
			aiowrite(sock,buf);
		}
		memset(buf, 0, sizeof(buf));
		strcpy(buf, "womenover");
		clear(sock,buf);
		if(identify=='D'){
			aioread(sock);
			memset(buf, 0, sizeof(buf));
			while((c=getchar())=='\n');
			buf[0]=c;
			//buf[0]=getchar();
			printf("buf:%s\n",buf);
			aiowrite(sock,buf);
			aioread(sock);
		}
		memset(buf, 0, sizeof(buf));
		strcpy(buf, "manover");
		clear(sock,buf);
		aioread(sock);
		aioread(sock);	
		aioread(sock);
		int count=buf[0]-'0';
		//printf("count=%d\n",count);
		//玩家讨论
		/*while(1){
			aioreadtest(sock);
			memset(buf, 0, sizeof(buf));
			char c;
			while((c=getchar())=='\n');
			buf[0]=c;
			fgets(&buf[1],BUFF_SIZE,stdin);
			printf("buf=%s\n",buf);
			aiowrite(sock,buf);
			aioreadtest(sock);
			if(!strcmp(buf,"discussover")) break;
		}*/
		while((c=getchar())=='\n');
		buf[0]=c;
		fgets(&buf[1],BUFF_SIZE,stdin);
		aiowrite(sock,buf);
		for(;count>0;count--) aioread(sock);
		memset(buf, 0, sizeof(buf));
		strcpy(buf, "discussover");
		clear(sock,buf);
		memset(buf, 0, sizeof(buf));
		while((c=getchar())=='\n');
		buf[0]=c;
		//buf[0]=getchar();
		printf("buf:%s\n",buf);
		aiowrite(sock,buf);
		aioread(sock);
	}
}

int main(int ac,char *av[]) 
{
	if(ac!=3){
		fprintf(stderr,"usage: %s [hostname] [port]\n", av[0]);
		exit(1);
	}
	int sock;
	struct sockaddr_in servadd;
	struct hostent *hp;
	sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock==-1){
		perror("cannot socket");
		return -1;
	}
	bzero(&servadd,sizeof(servadd));
	hp=gethostbyname(av[1]);
	if(hp==NULL) return -1;
	bcopy(hp->h_addr,(struct sockaddr *)&servadd.sin_addr,hp->h_length);
	servadd.sin_port=htons(atoi(av[2]));
	servadd.sin_family=AF_INET;
	if(connect(sock,(struct sockaddr *)&servadd,sizeof(servadd))!=0){
		perror("cannot connect");
		exit(1);
	}
	printf("欢迎来到狼人杀游戏（简单版）\n");
	printf("版本号1.0\n");
	printf("游戏规则：本游戏将有6名玩家参与，在游戏中每位玩家将扮演平民，狼人，女巫，预言家其中的一种身份，共有2名平民，2名狼人，1名女巫，1名预言家，服务器将担任法官的角色；黑天时根据服务器指示，狼人可以杀人，女巫可以选择使用解药或者毒药来解救或杀死一名玩家，每种药品有且只有1瓶，预言家每晚可以查看一名玩家的身份.\n");
	printf("等待玩家中,游戏即将开始...\n");
	aioread(sock);
	identify=buf[6];
	id=buf[0]-'0';  //change char to int
	memset(buf, 0, sizeof(buf));
	strcpy(buf, "OK");
	clear(sock,buf);  //clear client error input and game waiting
	game(sock);
    	close(sock); 
    	return 0; 
}                      
