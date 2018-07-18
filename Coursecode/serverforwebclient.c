#include <sys/types.h>       
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

main(int ac, char *av[]){
	int tcp_socket;
	struct sockaddr_in addbuf;
	int fd;
	char buf[512];
	int n;

	if(ac<2){
		printf("USE: CMD port_number\n");
		exit(1);
	}
	tcp_socket=socket(AF_INET, SOCK_STREAM, 0);

	if(tcp_socket==-1){
		perror("cannot socket");
		exit(1);
	}
	addbuf.sin_family=AF_INET;
	addbuf.sin_port=htons(atoi(av[1]));
	addbuf.sin_addr.s_addr=INADDR_ANY;

	if(bind(tcp_socket, (const struct 
					sockaddr *)&addbuf, sizeof(struct sockaddr_in))==-1){
		perror("cannot bind");
		exit(1);
	}

	if(listen(tcp_socket, 1)==-1){
		perror("cannot listen");
		exit(1);
	}

	while(1){
		fd=accept(tcp_socket, NULL, 0);
		while((n=read(fd, buf, sizeof(buf)))>0){
			printf("%.*s", n, buf);
		}
		close(fd);	
	}
}
