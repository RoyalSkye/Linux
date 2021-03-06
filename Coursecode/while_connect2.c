#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];
	portno = atoi(argv[2]);
	int noo=0;
	int i;
	for(i=0;i<10000;i++){
		if(0==	fork())
		{
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if (sockfd < 0) 
				error("ERROR opening socket");
			server = gethostbyname(argv[1]);
			bzero((char *) &serv_addr, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			bcopy((char *)server->h_addr, 
					(char *)&serv_addr.sin_addr.s_addr,
					server->h_length);
			serv_addr.sin_port = htons(portno);

			if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
				error("ERROR connecting");

			printf("connect %d\n", noo++);
			sleep(1000);
		}
	}
	sleep(10000);	
}


