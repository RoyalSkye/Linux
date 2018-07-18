#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <aio.h>
#include <signal.h>
#include <sys/time.h>
//#include <pthread.h>
//#include <sys/syscall.h> 

#define BACKLOG 6
#define BUFF_SIZE       1024    
#define SELECT_TIMEOUT  10       //select的timeout seconds

int id=1;
int gg=1;  //game over
char buffer[BUFF_SIZE];
int ret;
struct sigaction sig_act;
struct aiocb my_aiocb[BUFF_SIZE];
fd_set readfds;  //创建并初始化select需要的参数(这里仅监视read)，并把sock添加到fd_set中
fd_set readfds_bak; //backup for readfds(每次select之后会更新readfds)
char identify[6][2]={'A','T',     //A-平民  B-狼人 C-女巫 D-预言家
			'A','T',
			'B','T',	
			'B','T',
			'C','T',
			'D','T'};
struct user{
	int id;
	int alive;
	int sock;
	int antidote;
	int poison;
	int a;  //the position in the identify
	char identify;
};
struct user player[BACKLOG];

void setSockNonBlock(int sock){  //set sock to non-block
	int flags;
	flags = fcntl(sock,F_GETFL,0);
      if(flags < 0){
		perror("fcntl(F_GETFL) failed");
		exit(1);
        }
	if(fcntl(sock,F_SETFL,flags|O_NONBLOCK) < 0){
		perror("fcntl(F_SETFL) failed");
            exit(1);
        }
}
int updateMaxfd(fd_set fds, int maxfd){  //update maxfd
	int i;
	int new_maxfd = 0;
	for(i = 0; i <= maxfd; i++){
		if(FD_ISSET(i, &fds) && i > new_maxfd){
			new_maxfd = i;
		}
        }
	return new_maxfd;
}
void aio_completion_handler(int signo, siginfo_t *info, void *context)  //receive message from client
{
	//用来获取读aiocb结构的指针
    	struct aiocb *req;
    	int ret;
	printf("线程回调函数 ");
	if (info->si_signo == SIGIO){
		req = (struct aiocb *)info->si_value.sival_ptr;
		if (aio_error(req) == 0) {
			ret = aio_return(req);
			printf("ret=%d\n",ret);
			if(ret==0){
				id--;
				player[id-1].alive=0;
				identify[player[id-1].a][1]='T';
				printf("client: %d has disconnected!\n",id);
				//printf("player[id-1].sock:%d\n",player[id-1].sock);
				close(player[id-1].sock);
				FD_CLR(player[id-1].sock, &readfds_bak);
				return;
			}
			memset(buffer, 0, sizeof(buffer));
			strcpy(buffer,(const char *)req);
			printf("receive:%s\n",req->aio_buf);			
		}
	}
	return;
}
/*void aio_completion_handler1(int signo, siginfo_t *info, void *context)  //send message to client
{
	//用来获取读aiocb结构的指针
    	struct aiocb *req;
    	int ret;
	printf("线程回调函数 ");
	if (info->si_signo == SIGIO){
		req = (struct aiocb *)info->si_value.sival_ptr;
		if (aio_error(req) == 0) {
			ret = aio_return(req);
			//printf("ret=%d\n",ret);
			printf("send successfully:%s\n",req->aio_buf);
			printf("send successfully\n");
		}
	}
	return;
}*/
void aioread(int fd,int i){  //AIO with inform
	ret=0;
 	bzero((char *)&my_aiocb[i],sizeof(struct aiocb));
 	my_aiocb[i].aio_buf = malloc(BUFF_SIZE+1);
  	if(!my_aiocb[i].aio_buf) perror("malloc");
 	my_aiocb[i].aio_fildes = fd;
  	my_aiocb[i].aio_nbytes = BUFF_SIZE;
  	my_aiocb[i].aio_offset = 0;
	// Set up the signal handler
  	sigemptyset(&sig_act.sa_mask);
  	sig_act.sa_flags = SA_SIGINFO;
  	sig_act.sa_sigaction = aio_completion_handler;
	// Link the AIO request with the Signal Handler
  	my_aiocb[i].aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  	my_aiocb[i].aio_sigevent.sigev_signo = SIGIO;
  	my_aiocb[i].aio_sigevent.sigev_value.sival_ptr = &my_aiocb[i];
	// Map the Signal to the Signal Handler 
  	ret = sigaction(SIGIO,&sig_act,NULL);
  	ret = aio_read(&my_aiocb[i]);
  	if (ret < 0) perror("aio_read");
}
void aioread1(int fd){
	struct aiocb my_aiocb1;
	ret=0;
 	bzero((char *)&my_aiocb1,sizeof(struct aiocb));
 	my_aiocb1.aio_buf = malloc(BUFF_SIZE+1);
  	if(!my_aiocb1.aio_buf) perror("malloc");
 	my_aiocb1.aio_fildes = fd;
  	my_aiocb1.aio_nbytes = BUFF_SIZE;
  	my_aiocb1.aio_offset = 0;
  	ret = aio_read(&my_aiocb1);
  	if (ret < 0) perror("aio_read");
 	while (aio_error(&my_aiocb1) == EINPROGRESS);  //EINPROGRESS请求尚未完成, ECANCELLED请求被应用程序取消了
  	if((ret = aio_return(&my_aiocb1))>0){  //只有在 aio_error调用确定请求已经完成（可能成功，也可能发生了错误）之后才会调用这个函数 返回值就等价于同步情况中read或write 系统调用的返回值（所传输的字节数 error -1)
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer,(const char *)my_aiocb1.aio_buf);
		printf("ret:%d\n",ret);
		printf("receive:%s\n",my_aiocb1.aio_buf);
  	}else if(ret==0){
		printf("ret:%d\n",ret);
		int i;
		for(i=0;i<BACKLOG;i++){
			if(player[i].sock==fd){
				player[i].alive=0;
				identify[player[i].a][1]='T';
				printf("client: %d has disconnected!\n",i+1);
				close(player[i].sock);
				FD_CLR(player[i].sock, &readfds_bak);
				break;
			} 	
		}
		id--;
		return;
	}
}
void aiowrite(int fd,char *buf){
	struct aiocb my_aiocb;
	ret=0;
	bzero((char *)&my_aiocb,sizeof(struct aiocb));
	my_aiocb.aio_buf = malloc(BUFF_SIZE+1);
  	if(!my_aiocb.aio_buf) perror("malloc");
 	my_aiocb.aio_fildes = fd;
  	my_aiocb.aio_nbytes = BUFF_SIZE;    //if sizeof(buf)  only send sizeof(buf)8!
  	my_aiocb.aio_offset = 0;
	my_aiocb.aio_buf =buf;
	ret = aio_write(&my_aiocb);
    	if(ret < 0) perror("aio_write");
	while(aio_error(&my_aiocb) == EINPROGRESS);
	if((ret = aio_return(&my_aiocb))>0){
		printf("ret:%d\n",ret);
		printf("send successfully:%s\n",my_aiocb.aio_buf);
  	}else{
  		printf("send error!\n");
	}
}

int game_waiting(){
	if(id!=(BACKLOG+1)) return 0;
	else{	
		int i;
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "OK");
		for(i=0;i<BACKLOG;i++){
			aiowrite(player[i].sock,buffer);
		}
		return 1;
	}
}
int game_over1(){  //除了狼人以外的玩家 0--有存活  1--全体阵亡
	int i=0,flag=1;
	for(i=0;i<BACKLOG;i++){
		if(player[i].identify!='B'){
			if(player[i].alive==0) continue;
			else return 0;
		}
	}
	return 1;
}
int game_over2(){  //狼人 0--狼人有存活  1--狼人全体阵亡
	int i=0;
	for(i=0;i<BACKLOG;i++){
		if(player[i].identify=='B'){
			if(player[i].alive==0) continue;
			else return 0;
		}
	}
	return 1;
}
void game(){
	sleep(1);
	gg=1;
	int flag1=0;
	int flag2=0;
	while(gg){
		int i;
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "天黑请闭眼");
		for(i=0;i<BACKLOG;i++){
			if(player[i].alive) aiowrite(player[i].sock,buffer);
		}
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "狼人请睁眼,请选择要杀的玩家编号(1-6),可以自杀,但所有狼人目标须一致,不一致以第一个为准,不允许输入非法字符,否则视为弃权");
		for(i=0;i<BACKLOG;i++){
			if(player[i].alive) aiowrite(player[i].sock,buffer);
		}
		//receive狼人
		int target[2]={0,0};  //wolf target
		int res,timer;  //timer control the time of client input
		int maxfd=0,temp,count,wolf;
		timer=0;count=0;wolf=0;
		fd_set readfds1;
		fd_set readfds_bak1;
		FD_ZERO(&readfds1);
		struct timeval timeout;
		FD_ZERO(&readfds_bak1);
		for(i=0;i<BACKLOG;i++){
			if(player[i].identify=='B' && player[i].alive){
				FD_SET(player[i].sock,&readfds_bak1);
				wolf++;
				if(player[i].sock>maxfd) maxfd=player[i].sock;
			}
		}
		//printf("wolf=%d\n",wolf);
		while(timer<=10&&count<2){
			readfds1 = readfds_bak1;
			maxfd = updateMaxfd(readfds1, maxfd); 
			timeout.tv_sec = 5;
            	timeout.tv_usec = 0;
			res=select(maxfd + 1, &readfds1, NULL, NULL, &timeout);
			if(res>0){
				for(i=0;i<=maxfd;i++){
					if(!FD_ISSET(i,&readfds1)){
						continue;
                			}else{
						aioread1(i);
						temp=buffer[0]-'0';
						if(temp>0&&temp<7&&player[temp-1].alive){
							target[count++]=temp;
							if(wolf==1){ timer=10; printf("wolf=1"); break;}
							if(count>=2) break;
						}else{  //error input
							count++;
						}
					}
				}
			}
			timer++;
		}
		int target1=0;
		for(i=0;i<2;i++){
			if(target[0]==0||target[1]==0) target1=0;
			if(target[0]==0||target[1]!=0) target1=target[1];
			if(target[0]!=0||target[1]==0) target1=target[0];
			if(target[0]!=0||target[1]!=0){
				res=rand()%2;
				target1=target[res];
			}
		}
		printf("狼人选择的目标为:target=%d\n",target1);
		if(target1==0) printf("狼人选择目标无效\n");
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "wolfover");
		for(i=0;i<BACKLOG;i++){
			if(player[i].alive) aiowrite(player[i].sock,buffer);
		}
		//女巫
		int target2=0;  //毒药目标 1-6 correct 0 error
		int target3=0;  //解药 1使用解药 0不使用解药
		for(i=0;i<BACKLOG;i++){
			if(player[i].identify=='C') break;
		}
		if(player[i].alive && (player[i].antidote || player[i].poison)){
			memset(buffer, 0, sizeof(buffer));
			sprintf(buffer,"今晚被杀的玩家为 %d 号玩家(1-6,0代表狼人弃权或输入无效,是否使用解药?是否使用毒药?请输入毒药的使用对象玩家编号:(输入格式eg:100,输入非法视为弃权))",target1);
			aiowrite(player[i].sock,buffer);
			fd_set readfds;
			fd_set readfds_bak;
			FD_ZERO(&readfds);
			int timer=0,res;
			char target[4];
			struct timeval timeout;
			FD_ZERO(&readfds_bak);
			FD_SET(player[i].sock,&readfds_bak);
			while(timer<=10){
				readfds = readfds_bak;
				timeout.tv_sec = 5;
		    		timeout.tv_usec = 0;
				res=select(player[i].sock + 1, &readfds, NULL, NULL, &timeout);
				if(res>0&&(FD_ISSET(player[i].sock,&readfds))){
					aioread1(player[i].sock);
					strncpy(target,buffer,3);
					if(player[i].poison){  //have poison
						target2=target[2]-'0';
						if(((target[1]-'0')==1)&&target2>0&&target2<7) player[i].poison=0; 
						else target2=0;
					}
					if(player[i].antidote){  //have antidote
						target3=target[0]-'0';
						if(target3==1){
							target3=1;
							player[i].antidote=0;
						}
						else target3=0;
					}
					//printf("target:%s\n",target);
					break;
				}
				timer++;
			}
		}
		
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "womenover");
		for(i=0;i<BACKLOG;i++){
			if(player[i].alive) aiowrite(player[i].sock,buffer);
		}
		//预言家
		int target4; //预言家查看目标 1-6 T 0 error
		for(i=0;i<BACKLOG;i++){
			if(player[i].identify=='D') break;
		}
		if(player[i].alive){
			memset(buffer, 0, sizeof(buffer));
			strcpy(buffer, "请输入您想查看身份的玩家编号(1-6),输入非法视为弃权");
			aiowrite(player[i].sock,buffer);
			fd_set readfds;
			fd_set readfds_bak;
			FD_ZERO(&readfds);
			FD_ZERO(&readfds_bak);
			int timer=0,res;
			struct timeval timeout;
			FD_SET(player[i].sock,&readfds_bak);
			while(timer<=10){
				readfds = readfds_bak;
				timeout.tv_sec = 5;
		    		timeout.tv_usec = 0;
				res=select(player[i].sock + 1, &readfds, NULL, NULL, &timeout);
				if(res>0&&(FD_ISSET(player[i].sock,&readfds))){
					aioread1(player[i].sock);
					target4=buffer[0]-'0';
					if(target4>0&&target4<7); else target4=0;
					if(target4!=0){
						memset(buffer, 0, sizeof(buffer));
						sprintf(buffer,"您要查看的玩家编号为%d号玩家,该玩家身份为%c,A-平民 B-狼人 C-女巫 D-预言家",target4,player[target4-1].identify);
						aiowrite(player[i].sock,buffer);
					}else{
						memset(buffer, 0, sizeof(buffer));
						strcpy(buffer, "输入非法");
						aiowrite(player[i].sock,buffer);
					}
					break;
				}
				timer++;
			}
		}
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "manover");
		for(i=0;i<BACKLOG;i++){
			if(player[i].alive) aiowrite(player[i].sock,buffer);
		}
		//处理死亡玩家and发布结果
		//printf("target1=%d\n",target1);
		//printf("target2=%d\n",target2);
		//printf("target3=%d\n",target3);
		if((target1!=0&&target3==0) || (target2!=0)){
			if((target1!=0&&target3==0) && (target2!=0)){
				memset(buffer, 0, sizeof(buffer));
				sprintf(buffer,"%d号玩家阵亡 %d号玩家阵亡",target1,target2);
				for(i=0;i<BACKLOG;i++){
					if(player[i].alive) aiowrite(player[i].sock,buffer);
				}
				player[target1-1].alive=0;
				close(player[target1-1].sock);
				player[target2-1].alive=0;
				close(player[target2-1].sock);
			}else if(target1!=0&&target3==0){
				memset(buffer, 0, sizeof(buffer));
				sprintf(buffer,"%d号玩家阵亡",target1);
				for(i=0;i<BACKLOG;i++){
					if(player[i].alive) aiowrite(player[i].sock,buffer);
				}
				player[target1-1].alive=0;
				close(player[target1-1].sock);
			}else if(target2!=0){
				memset(buffer, 0, sizeof(buffer));
				sprintf(buffer,"%d号玩家阵亡",target2);
				for(i=0;i<BACKLOG;i++){
					if(player[i].alive) aiowrite(player[i].sock,buffer);
				}
				player[target2-1].alive=0;
				close(player[target2-1].sock);
			}
		}else{
			memset(buffer, 0, sizeof(buffer));
			strcpy(buffer, "无事发生,没有玩家死亡");
			for(i=0;i<BACKLOG;i++){
				if(player[i].alive) aiowrite(player[i].sock,buffer);
			}
		}
		flag1=game_over1();
		flag2=game_over2();
		if(flag1 || flag2){
			gg=0;
			break;
		}
		//玩家讨论
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "请剩余玩家发表意见");
		for(i=0;i<BACKLOG;i++){
			if(player[i].alive) aiowrite(player[i].sock,buffer);
		}
		FD_ZERO(&readfds);
		FD_ZERO(&readfds_bak);
		maxfd=0;
		for(i=0;i<BACKLOG;i++){
			if(player[i].alive){
				FD_SET(player[i].sock,&readfds_bak);
				if(player[i].sock>maxfd) maxfd=player[i].sock;
			}
		}
		char temp1[BUFF_SIZE];
		count=0;
		for(i=0;i<BACKLOG;i++){
			if(player[i].alive) count++;
		} 
		memset(buffer, 0, sizeof(buffer));
		sprintf(buffer,"%d",count);
		for(i=0;i<BACKLOG;i++){
			if(player[i].alive) aiowrite(player[i].sock,buffer);
		}
		int flag=0;
		while(flag<count){
			readfds = readfds_bak;
			maxfd = updateMaxfd(readfds, maxfd);  
			timeout.tv_sec = 5;
			timeout.tv_usec = 0;
			res=select(maxfd + 1, &readfds, NULL, NULL, &timeout);
			if(res>0){
				for (i = 0;i <= maxfd;i++){
					if(FD_ISSET(i,&readfds)){
						flag++;
						aioread1(i);
						strcpy(temp1,buffer);
						int j,user;
						for(j=0;j<BACKLOG;j++) if(player[j].sock==i){ user=player[j].id; break;}
						for(j=0;j<BACKLOG;j++){
							if(player[j].alive && player[j].sock!=i){
								memset(buffer, 0, sizeof(buffer));
								sprintf(buffer,"from%d号玩家:%s",user,temp1);
								aiowrite(player[j].sock,buffer);
							}
						}
						FD_CLR(i, &readfds_bak);
                			}
				}
			}
		}
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "discussover");
		for(i=0;i<BACKLOG;i++){
			if(player[i].alive) aiowrite(player[i].sock,buffer);
		}
		//投票踢出一名玩家
		int ticket[6]={0,0,0,0,0,0};
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "根据其他玩家的发言,请选择你认为有威胁的玩家(1-6),非法输入将视为弃权,如果两名玩家票数相同,将随机选择");
		for(i=0;i<BACKLOG;i++){
			if(player[i].alive) aiowrite(player[i].sock,buffer);
		}
		FD_ZERO(&readfds);
		FD_ZERO(&readfds_bak);
		maxfd=0;
		for(i=0;i<BACKLOG;i++){
			if(player[i].alive){
				FD_SET(player[i].sock,&readfds_bak);
				if(player[i].sock>maxfd) maxfd=player[i].sock;
			}
		}
		count=0;
		for(i=0;i<BACKLOG;i++){
			if(player[i].alive) count++;
		} 
		flag=0;
		while(flag<count){
			readfds = readfds_bak;
			maxfd = updateMaxfd(readfds, maxfd);  
			timeout.tv_sec = 5;
			timeout.tv_usec = 0;
			res=select(maxfd + 1, &readfds, NULL, NULL, &timeout);
			if(res>0){
				for (i = 0;i <= maxfd;i++){
					if(FD_ISSET(i,&readfds)){
						flag++;
						aioread1(i);
						int tickettemp=buffer[0]-'0';
						if(tickettemp>0&&tickettemp<7&&player[tickettemp-1].alive) ticket[tickettemp-1]++;
						FD_CLR(i, &readfds_bak);
					}
				}
			}
		}
		for(i=0;i<BACKLOG;i++){
			printf("%d号玩家=%d\n",i+1,ticket[i]);
		}
		int max=0;
		count=0;
		int ticket1[BACKLOG]={0,0,0,0,0,0};
		for(i=0;i<BACKLOG;i++){
			if(max<ticket[i]) max=ticket[i];
		}
		for(i=0;i<BACKLOG;i++){
			if(ticket[i]==max){
				ticket1[count++]=i+1;  //id
			}
		}
		int rand1=rand()%count;
		printf("rand=%d\n",rand1);
		int target5=ticket1[rand1];
		memset(buffer, 0, sizeof(buffer));
		sprintf(buffer,"大多数玩家选择将%d号玩家踢出游戏!",target5);
		for(i=0;i<BACKLOG;i++){
			if(player[i].alive) aiowrite(player[i].sock,buffer);
		}
		//处理死亡玩家
		if(player[target5-1].alive){
			player[target5-1].alive=0;
			close(player[target5-1].sock);
		}
		flag1=game_over1();
		flag2=game_over2();
		if(flag1 || flag2){
			gg=0;
			break;
		}
	}
	printf("game over!\n");
	if(flag2){  //狼人战败
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "game over!狼人战败,平民胜利!您获得成功!");
		int i;
		for(i=0;i<BACKLOG;i++){
			if(player[i].alive){
				aiowrite(player[i].sock,buffer);
				player[i].alive=0;
				close(player[i].sock);
			}
		}
		return;
	}else if(flag1){  //狼人获胜
		int i;
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "game over!平民战败,狼人获胜,您获得成功!");
		for(i=0;i<BACKLOG;i++){
			if(player[i].alive){
				aiowrite(player[i].sock,buffer);
				player[i].alive=0;
				close(player[i].sock);
			}
		}
		return;
	}		
}

int main(int ac, char *av[]){
	int port,sock;
	if(ac!=2){
		fprintf(stderr, "usage: %s [port]\n", av[0]);
            exit(1);
	}
	port = atoi(av[1]);
      if((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket failed, ");
            exit(1);
        }
	int new = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &new, sizeof(int))) {  //allow bind the address or port which is already in use. (4)optval：指针 指向存放选项待设置的新值的缓冲区。
		perror("setsockopt failed");
		exit(1);
        }
	setSockNonBlock(sock);
	struct sockaddr_in bind_addr;
	memset(&bind_addr,0,sizeof(bind_addr));
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);  
	bind_addr.sin_port = htons(port);               
	if(bind(sock,(struct sockaddr *)&bind_addr,sizeof(bind_addr))==-1){
		perror("bind failed, ");
            exit(1);
        }
	if(listen(sock, BACKLOG)==-1){
            perror("listen failed.");
            exit(1);
        }
	struct timeval timeout;
	int maxfd;
	maxfd = sock;
	FD_ZERO(&readfds);
	FD_ZERO(&readfds_bak);
	FD_SET(sock,&readfds_bak);
	int new_sock,i,res;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len;
	char client_ip_str[INET_ADDRSTRLEN];
	int recv_size,ret;
	while(1){   //循环接受client请求
		readfds = readfds_bak;  //reset, select之后readfds和timeout的值都会被修改,没有相应状态的fd被移除
            maxfd = updateMaxfd(readfds, maxfd);  
            timeout.tv_sec = SELECT_TIMEOUT;
            timeout.tv_usec = 0;
            res = select(maxfd + 1, &readfds, NULL, NULL, &timeout);
		if(res<0){
			if(errno==EINTR) continue;   //select failed: Interrupted system call
			perror("select failed");
			exit(1);
		}else if (res == 0){ //timeout
			fprintf(stderr, "no socket is ready for read within %d seconds\n", SELECT_TIMEOUT);
			continue;
		}
            for (i = 0;i <= maxfd;i++){
			if(!FD_ISSET(i,&readfds)){
				continue;
                	}
			if(i == sock){  //server socket, accept
				client_addr_len = sizeof(client_addr);
                    	new_sock = accept(sock, (struct sockaddr *) &client_addr, &client_addr_len);
                    	if(new_sock == -1){
                        	perror("accept failed");
                        	exit(1);
                    		}
				if(id>BACKLOG){  //server is full,return error meaasge to client
					memset(buffer, 0, sizeof(buffer));
					strcpy(buffer, "game is running, the number of server player is full!");
					aiowrite(new_sock,buffer);
					close(new_sock);
					continue;
				}
				int a;
				do{
					a=rand()%6;  //[0,5]	
					player[id-1].identify=identify[a][0];
				}while(identify[a][1]=='F');
				//printf("a=%d\n",a);
				identify[a][1]='F';
				player[id-1].a=a;
				player[id-1].alive=1;
				player[id-1].sock=new_sock;
				player[id-1].antidote=0;
				player[id-1].poison=0;
				if(player[id-1].identify=='C'){
					player[id-1].antidote=1;
					player[id-1].poison=1;
				}
				player[id-1].id=id++;
                    	if(!inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip_str, sizeof(client_ip_str))){
                        	perror("inet_ntop failed");
                        	exit(1);
                    		}
                    	printf("accept a client from: %s\n", client_ip_str);
                    	setSockNonBlock(new_sock);
                    	if(new_sock > maxfd){
                       		maxfd = new_sock;
                    		}
                    	FD_SET(new_sock, &readfds_bak);
				memset(buffer, 0, sizeof(buffer));
				sprintf(buffer,"%d %d %d %c",player[id-2].id,player[id-2].alive,player[id-2].sock,player[id-2].identify);
				aiowrite(new_sock,buffer);
				if(game_waiting()){
					game();
					exit(0);
				}
                	}else{  //client socket, can be read, send message to server
				memset(buffer, 0, sizeof(buffer));
				aioread(i,0);
				//aioread1(i);
				sleep(1);
                    	//FD_CLR(i, &readfds_bak);
                	}
            	}
        }
	return 0;
}



