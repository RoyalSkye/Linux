#include <stdio.h>
#include <pthread.h>

void *sell(void *);

int ticket;
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;

main(){
	pthread_t t1, t2;
	ticket=100;
	
	pthread_create(&t1, NULL, sell, "Shenyang");
	pthread_create(&t2, NULL, sell, "Shenyangbei");
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);	
}

void *sell(void *station){
	while(1){
		pthread_mutex_lock(&lock);
		if(ticket>0){	
			printf("%s selled no. %d ticket\n", (char *)station, ticket);
			ticket--;
			pthread_mutex_unlock(&lock);
		}else{
			pthread_mutex_unlock(&lock);
			pthread_exit(0);
		}
	}
}
