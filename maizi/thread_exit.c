/*####################################################
# File Name: thread_exit.c
# Author: YanWei
# Email: nameqiaohe@126.com
# Create Time: 2016-05-05 00:01:24
# Last Modified: 2016-05-05 00:01:24
####################################################*/

#include "apue.h"

void *thread_func(void *arg){
	if(strcmp("1", (char *)arg) == 0){
		printf("New Thread===return\n");
		return (void *)1;
	}
	if(strcmp("2", (char *)arg) == 0){
		printf("New Thread===pthread_exit\n");
		pthread_exit((void *)2);
	}
	if(strcmp("3", (char *)arg) == 0){
		printf("New Thread===exit\n");
		exit(3);
	}
}

int main(int argc, char *argv[]){
	int err;
	pthread_t tid;

	err = pthread_create(&tid, NULL, thread_func, (void *)argv[1]);
	assert(!err);

	pthread_join(tid, NULL);

	printf("Main Thread return\n");
	return 0;
}
