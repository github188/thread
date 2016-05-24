/*####################################################
# File Name: thread_not_sync.c
# Author: YanWei
# Email: nameqiaohe@126.com
# Create Time: 2016-05-25 06:22:34
# Last Modified: 2016-05-25 06:22:34
# 不进行任何的 同步操作，会出现 当两个或多个线程同时访问数据时 发生数据出现异常的情况
# 
# 现在，使用pthread_mutex_lock、pthread_mutex_unlock进行对数据的加锁、解锁
####################################################*/

#include "apue.h"

struct student{
	int id;
	int age;
	int name;
}stu;

int i;//global

pthread_mutex_t mutex;

void *thread_func1(void *arg){
	while(1){
		pthread_mutex_lock(&mutex);
		stu.id = i;
		stu.age = i;
		stu.name = i;
		i++;

		if(stu.id != stu.age || stu.id != stu.name || stu.age != stu.name){
			printf("%d, %d, %d\n", stu.id, stu.age, stu.name);
			break;
		}
		pthread_mutex_unlock(&mutex);
	}

	return (void *)0;
}
void *thread_func2(void *arg){
	while(1){
		pthread_mutex_lock(&mutex);
		stu.id = i;
		stu.age = i;
		stu.name = i;
		i++;

		if(stu.id != stu.age || stu.id != stu.name || stu.age != stu.name){
			printf("%d, %d, %d\n", stu.id, stu.age, stu.name);
			break;
		}
		pthread_mutex_unlock(&mutex);
	}

	return (void *)0;
}

int main(void){
	pthread_t tid1, tid2;
	int err;

	err = pthread_mutex_init(&mutex, NULL);
	if(err != 0){
		printf("init mutex failed\n");
		return -1;
	}

	err = pthread_create(&tid1, NULL, thread_func1, NULL);
	if(err != 0){
		printf("create new thread failed\n");
		return;
	}

	err = pthread_create(&tid2, NULL, thread_func2, NULL);
	if(err != 0){
		printf("create new thread failed\n");
		return;
	}
	
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	return 0;
}
