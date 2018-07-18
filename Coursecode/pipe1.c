#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

main(){
	int pipefd[2];
	char buf[512];
	int n;

	pipe(pipefd);

	write(pipefd[1], "Hello world\n", 12);
	n=read(pipefd[0], buf, sizeof(buf));

	printf("%.*s\n", n, buf);
}
