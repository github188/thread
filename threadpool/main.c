/*####################################################
# File Name: main.c
# Author: YanWei
# Email: nameqiaohe@126.com
# Create Time: 2016-05-26 06:47:16
# Last Modified: 2016-05-26 06:47:16
####################################################*/
#include "threadpool.h"

void thread_func(void *arg){
	int *data = (int *)arg;
	printf("%d\n", *data);
}

int main(int argc, char *argv[]){
	threadpool *pool = (threadpool *)malloc(sizeof(threadpool));
	threadpool_init(pool, 2);

	int i;
	int tmp[3];

	for(i; i < 5; i++){
		tmp[i] = i;
		threadpool_add_runner(pool, thread_func, &tmp[i]);
	}

	sleep(1);
	threadpool_destroy(&pool);
	printf("main ===> %p\n", pool);
	printf("main ===> test over\n");

	return 0;
}
