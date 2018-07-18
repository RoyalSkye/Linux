#include <pthread.h>
#include <stdio.h>

int ticketnum=100;

void *sellticket(void *arg){
	while(1){
		if(ticketnum>0){
			printf("%s sold no. %d ticket\n", (char *)arg, ticketnum);
	//		sleep(1);
			ticketnum--;
		}
	}
}

main(){
	pthread_t t1, t2;
	pthread_create(&t1, NULL, sellticket, (void *)"Shenyangnan");
	pthread_create(&t2, NULL, sellticket, (void *)"Shenyangbei");

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
}
