#include <termios.h>
#include <unistd.h>
#include <stdio.h>

main(){
	struct termios buf, save;
	char name[48];
	char passwd[48];
	printf("Input name:");
	scanf("%s", name);
	printf("Input passwd:");
	tcgetattr(0, &buf);
	save=buf;
	buf.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &buf);
	scanf("%s", passwd);	
	tcsetattr(0, TCSANOW, &save);
	printf("Your name is %s, passwd is %s\n",
		name, passwd);
}

