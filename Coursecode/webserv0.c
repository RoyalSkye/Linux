#include <sys/types.h>       
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

main(int ac, char *av[]){
	int tcp_socket;
	struct sockaddr_in addbuf;
	int fd;
	char buf[1024];
	int n;
	char cmd[512];
	char filename[512];

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

	if(bind(tcp_socket, (const struct sockaddr *)&addbuf, sizeof(struct sockaddr_in))==-1){
		perror("cannot bind");
		exit(1);
	}

	if(listen(tcp_socket, 1)==-1){
		perror("cannot listen");
		exit(1);
	}

	while(1){
		int rv_fork;
		fd=accept(tcp_socket, NULL, 0);	

		rv_fork=fork();
		if(rv_fork==0){
			read(fd, buf, sizeof(buf));
			sscanf(buf, "%s%s", cmd, filename);
			if(strcmp(cmd, "GET")!=0){
				write(fd, "HTTP/1.0 400 Bad Request\r\n\r\n", strlen("HTTP/1.0 400 Bad Request\r\n\r\n"));	
				close(fd);
				continue;
			}else{
				int reqfd;
				if((reqfd=open(filename+1, O_RDONLY))==-1){
					write(fd, "HTTP/1.0 404 Page Not Found\r\n\r\n", strlen("HTTP/1.0 404 Page Not Found\r\n\r\n"));	
					close(fd);
					continue;
				}else{
					write(fd, "HTTP/1.0 200 OK\r\n\r\n", strlen("HTTP/1.0 200 OK\r\n\r\n"));	
					while((n=read(reqfd, buf, sizeof(buf)))>0){
						write(fd, buf, n);
					}
					close(reqfd);
					close(fd);
					continue;
				}	
			}
		}else{
			close(fd);
		}
	}
}
