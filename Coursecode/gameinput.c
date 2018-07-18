#include <termios.h>
#include <unistd.h>
#include <stdio.h>

main(){
	struct termios buf, save;
	char c;
	tcgetattr(0, &buf);
	save=buf;
	buf.c_lflag &= ~ECHO;
	buf.c_lflag &= ~ICANON;
	tcsetattr(0, TCSANOW, &buf);
	while(scanf("%c", &c)>0){
		printf("%d", c);
		fflush(stdout);
	}	
	tcsetattr(0, TCSANOW, &save);
}

