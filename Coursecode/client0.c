#include <sys/types.h> 
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>

main(int ac, char *av[]){
	int tcp_socket;
	struct sockaddr_in addbuf;
	char buf[512];
	int n;

	if(ac<3){
		printf("USE: CMD ip_addr port_number\n");
		exit(1);
	}
	if((tcp_socket= socket(AF_INET, SOCK_STREAM, 0))==-1){
		perror("cannot socket");
		exit(1);
	}

	addbuf.sin_family=AF_INET;
	addbuf.sin_port=htons(atoi(av[2]));
	addbuf.sin_addr.s_addr=inet_addr(av[1]);

	if(connect(tcp_socket, (const struct sockaddr *)&addbuf, sizeof(struct sockaddr_in))==-1){
		printf("cannot connect");
		exit(1);
	}

	n=read(tcp_socket, buf, sizeof(buf));

	printf("%.*s\n", n, buf);

	close(tcp_socket);
}	
