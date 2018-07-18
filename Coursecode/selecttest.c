#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main (void)
{
	fd_set rfds;
	struct timeval tv;
	int retval;

	int fd1, fd2;

	fd1 = open ("/dev/pts/1", O_RDONLY);
	fd2 = open ("/dev/input/mice", O_RDONLY);	//use root to open

	while(1){
		/* Watch fd1 and fd2 to see when it has input. */
		FD_ZERO (&rfds);
		FD_SET (fd1, &rfds);
		FD_SET (fd2, &rfds);

		/* Wait up to five seconds. */
		tv.tv_sec = 5;
		tv.tv_usec = 0;

		retval = select (1 + (fd1 > fd2 ? fd1 : fd2), &rfds, NULL, NULL, &tv);
		/* Don’t rely on the value of tv now! */

		if (retval == -1)
			perror ("select()");
		else if (retval)
		{
			printf ("Data is available now.\n");
			if(FD_ISSET (fd1, &rfds)){
				char buf[512];
				int n=read(fd1, buf, 512);

				printf("%.*s", n, buf);
			} 
			if(FD_ISSET (fd2, &rfds)){
				char buf[512];
				int n=read(fd2, buf, 512);

				printf("%.*s", n, buf);
			} 
		}
		else
			printf ("No data within five seconds.\n");
	}
	exit (EXIT_SUCCESS);
}
