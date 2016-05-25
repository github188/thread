/*####################################################
# File Name: producer_consumer_pthread_cond.c
# Author: YanWei
# Email: nameqiaohe@126.com
# Create Time: 2016-05-25 21:40:24
# Last Modified: 2016-05-25 21:40:24
####################################################*/
#include "apue.h"

#define BUFFER_SIZE 2
#define PRODUCT_SIZE 5
#define OVER (-1)	//the flag of last data

struct products{
	int buffer[BUFFER_SIZE];
	pthread_mutex_t lock;
	int writepos;
	int readpos;
	pthread_cond_t not_full;
	pthread_cond_t not_empty;
};

struct products buffer;// for storage the data/products

void init(struct products *prodc){
	//init mutex and cond
	pthread_mutex_init(&(prodc->lock), NULL);
	pthread_cond_init(&(prodc->not_empty), NULL);
	pthread_cond_init(&(prodc->not_full), NULL);

	prodc->writepos = 0;//at start
	prodc->readpos = 0;//at start
}

void put(struct products *prodc, int data){
	pthread_mutex_lock(&(prodc->lock));//get mutex ===> lock

	/* when (writepos + 1) is equal readpos, means it's full now, can't put data into it right now, block here */
	//当writepos+1 与 readpos 相等时，表示 满了，不能再写入了
	while((prodc->writepos+1)%BUFFER_SIZE == prodc->readpos){
		printf("\tproducer wait for condition : not_full\n");
		pthread_cond_wait(&(prodc->not_full), &(prodc->lock));
	}
	//put data into buffer until the condition is not_full
	prodc->buffer[prodc->writepos] = data;
	prodc->writepos++;//step forward writepos

	//if writepos is greater than BUFFER_SIZE, set it zero
	if(prodc->writepos >= BUFFER_SIZE){
		prodc->writepos = 0;
	}

	/* there is one data at least after put data into buffer, so it's not_empty, other thread can get data right now, send this signal weak up other thread */
	pthread_cond_signal(&(prodc->not_empty));

	pthread_mutex_unlock(&(prodc->lock));//release mutex ===> unlock
}

int get(struct products *prodc){
	int data;
	pthread_mutex_lock(&(prodc->lock));//get mutex ===> lock

	//if writepos is equal readpos, means it's not empty, other thread can't get data right now, block here
	//当 writepos 与 readpos 相等时，表示空了，不能再读取了
	while(prodc->writepos == prodc->readpos){
		printf("consumer wait for condition : not_empty\n");
		pthread_cond_wait(&(prodc->not_empty), &(prodc->lock));//wait for condition : not_empty, block here
	}

	/*1. writepos is not eauql readpos, means not_empty ===> can get data right now
	 *2. get the condition : not_empty, can get data now */
	data = prodc->buffer[prodc->readpos];//get data from buffer at the readpos
	prodc->readpos++;//step forward readpos

	if(prodc->readpos == BUFFER_SIZE){
		prodc->readpos = 0;
	}

	//there is a room for put data after get data, so it's not_full, other therad can put data right now, send this signal weak up other thread
	pthread_cond_signal(&(prodc->not_full));
	pthread_mutex_unlock(&(prodc->lock));//release mutex ===> unlock

	return data;
}

void *producer(void *data){
	int num;
	for(num = 0; num < PRODUCT_SIZE; num++){
		printf("\tproducer sleep 1 second...\n");
		sleep(1);
		printf("\tput the %dth product\n", num);

		put(&buffer, num);
	}
	for(num = 5; num < PRODUCT_SIZE*2; num++){
		printf("\tproducer sleep 3 seconds...\n");
		sleep(3);
		printf("\tput the %dth product\n", num);
		
		put(&buffer, num);
	}
	put(&buffer, OVER);
	printf("producer stopped\n");
	
	return NULL;
}

void *consumer(void *data){
	int pd = 0;
	while(1){
		printf("consumer sleep 2 seconds\n");
		sleep(2);
		pd = get(&buffer);
		printf("get the %dth product\n", pd);

		if(pd == OVER){
			break;
		}
	}
	printf("consumer stopped\n");
	return NULL;
}

int main(int argc, char *argv[]){
	pthread_t tid1, tid2;
	void *retval;

	init(&buffer);

	pthread_create(&tid1, NULL, producer, NULL);//producer thread
	pthread_create(&tid2, NULL, consumer, NULL);//consumer thread

	pthread_join(tid1, &retval);
	pthread_join(tid2, &retval);

	return 0;
}

/*
 生产者===消费者
 3个对象：
 	互斥锁， 配合条件变量使用
	空间 非空条件变量：
		消费者线程 在 空间没有产品时 等待这一条件变量
		生产者线程 在生成产品后 通知此条件变量
	空间 非满条件变量：
		生产者线程 在 空间满时 等待此条件变量
		消费者线程 在 消耗产品后 通知此条件变量

生产者线程 流程：
	（1）申请互斥锁，若 互斥锁 已被锁定，则等待
	（2）测试 空间 非满条件变量
	（3）若 条件满足，执行操作，完成后解锁 互斥锁
	（4）若 第二步 不满足，阻塞等待 非满条件变量

消费者线程 流程：
	（1）申请互斥锁，若已被锁定，则阻塞
	（2）测试 空间 非空
	（3）若 满足条件，则执行操作，完成后解锁
	（4）若 第二步 不满足，阻塞当前线程，等待 空间 非空条件
 * */
