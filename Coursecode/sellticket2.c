#include <pthread.h>
#include <stdio.h>

int ticketnum=100;

pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;

void *sellticket(void *arg){
	while(1){
		pthread_mutex_lock(&mutex_lock);
		if(ticketnum>0){
			printf("%s sold no. %d ticket\n", (char *)arg, ticketnum);
			sleep(1);
			ticketnum--;
		}
		pthread_mutex_unlock(&mutex_lock);
	}
}

main(){
	pthread_t t1, t2;
	pthread_create(&t1, NULL, sellticket, (void *)"Shenyangnan");
	pthread_create(&t2, NULL, sellticket, (void *)"Shenyangbei");

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
}
