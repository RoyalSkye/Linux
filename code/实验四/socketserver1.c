#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <strings.h>
#define HOSTLEN 256
#define BACKLOG 1
int make_server_socket(int);
int make_server_socket_q(int,int);
int make_server_socket(int portnum){
	make_server_socket_q(portnum,BACKLOG);	
}
int make_server_socket_q(int portnum,int backlog){
	struct sockaddr_in saddr;
	struct hostent *hp; //this is part of our
	char hostname[HOSTLEN]; //address
	int sock_id;
	sock_id=socket(PF_INET,SOCK_STREAM,0);	
	if(sock_id==-1){
		perror("cannot socket");
		return -1;
	}
	bzero((void*)&saddr,sizeof(saddr));
	gethostname(hostname,HOSTLEN);
	//printf("%s\n",hostname);
	hp=gethostbyname(hostname);
	if(hp==NULL){
		herror("hp is null:");  //etc/hosts
		return -1;
	}
	/*struct hostent { 
 		char  *h_name;            official name of host
               char **h_aliases;          alias list 
               int    h_addrtype;         host address type 
               int    h_length;           length of address 
               char **h_addr_list;        list of addresses
        }

	struct sockaddr_in{
        short sin_family;
        unsigned short sin_port;  Port number(必须要采用网络数据格式,普通数字可以用htons()函数转换成网络数据格式的数字)
        struct in_addr sin_addr;  IP address in network byte order（Internet address）
        unsigned char sin_zero[8];
	};*/
	bcopy(hp->h_addr,(struct sockaddr *)&saddr.sin_addr,hp->h_length);
	//saddr.sin_addr.s_addr=INADDR_ANY;
	saddr.sin_port=htons(portnum);//htons converts integer variables from host byte order to network byte order
	saddr.sin_family=AF_INET;
	if(bind(sock_id,(struct sockaddr *)&saddr,sizeof(saddr))!=0){
		perror("cannot bind");
		return -1;	
	}
	if(listen(sock_id,backlog)!=0){
		perror("cannot listen");
		return -1;	
	}
	return sock_id;	
}
main(int ac,char *av[]){
	if(ac!=2){
		perror("USE: CMD port_number\n");
		exit(1);
	}
	int fd,sock;
	sock=make_server_socket(atoi(av[1]));
	if(sock==-1){
		exit(1);
	}	
	while(1){
		fd=accept(sock,NULL,0);
		if(fd==-1) break;
		printf("This is server,accept successfully!\n");
		close(fd);
	}
}
