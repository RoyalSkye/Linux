#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <time.h>
#include <strings.h>
int connect_to_server(char *,int);
int connect_to_server(char *host,int portnum){
	//printf("host:%s\n",host);
	int sock;
	struct sockaddr_in servadd;
	struct hostent *hp;
	sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock==-1){
		perror("cannot socket");
		return -1;
	}
	bzero(&servadd,sizeof(servadd));
	hp=gethostbyname(host);
	if(hp==NULL) return -1;
	bcopy(hp->h_addr,(struct sockaddr *)&servadd.sin_addr,hp->h_length);
	servadd.sin_port=htons(portnum);
	servadd.sin_family=AF_INET;
	if(connect(sock,(struct sockaddr *)&servadd,sizeof(servadd))!=0){
		perror("cannot connect");
		return -1;
	}
	return sock;
}
main(int ac,char *av[]){
	if(ac!=3){
		perror("USE: CMD ip_addr port_number\n");
		exit(1);	
	}
	int fd;
	fd=connect_to_server(av[1],atoi(av[2]));
	if(fd==-1){
		exit(1);
	}
	printf("This is client,connect successfully!\n");
	close(fd);		
}
