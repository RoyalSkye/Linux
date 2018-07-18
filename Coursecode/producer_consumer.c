#include <pthread.h>
#include <stdio.h>

#define MAX 5

int buf[MAX]={0};
int num=0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t flag = PTHREAD_COND_INITIALIZER;

void *producer(void *arg){
	while(1){
		pthread_mutex_lock(&lock);
		if(num<MAX){
			buf[num]=1;
			printf("produce no. %d item\n", num);
			num++;
			pthread_cond_signal(&flag);	
		}else{
			pthread_cond_wait(&flag, &lock);
		}	
		pthread_mutex_unlock(&lock);
	}
}

void *consumer(void *arg){
	while(1){
		pthread_mutex_lock(&lock);
		if(num>0){
			num--;
			buf[num]=0;
			printf("consume no. %d item\n", num);
			pthread_cond_signal(&flag);	
		}else{
			pthread_cond_wait(&flag, &lock);
		}	
		pthread_mutex_unlock(&lock);
	}
}

main(){
        pthread_t t1, t2;
        pthread_create(&t1, NULL, producer, NULL);
        pthread_create(&t2, NULL, consumer, NULL);

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
}

