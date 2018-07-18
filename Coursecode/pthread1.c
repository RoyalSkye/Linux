#include <pthread.h>
#include <stdio.h>

void *f(void *arg){
	int i;
	for(i=0;i<5;i++){
		printf("%s\n", (char *) arg);
		sleep(1);
	}
}

main(){
	pthread_t t1, t2;
	pthread_create(&t1, NULL, f, (void *)"hello");
	pthread_create(&t2, NULL, f, (void *)"world");

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
}
