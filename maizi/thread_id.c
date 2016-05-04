/*####################################################
# File Name: thread_id.c
# Author: YanWei
# Email: nameqiaohe@126.com
# Create Time: 2016-05-04 20:57:32
# Last Modified: 2016-05-04 20:57:32
####################################################*/
//线程共享进程id
#include "apue.h"

void print_id(const char *str){
	pid_t pid;
	pthread_t tid;

	pid = getpid();
	tid = pthread_self();

	printf("%s pid = %u\t tid = %u\t tid = 0x%x\n", str, (unsigned int)pid, (unsigned int)tid, (unsigned int)tid);
}

void *thread_func(void *arg){
	print_id("New Thread: ");
	return (void *)0;
}

int main(void){
	int err;
	pthread_t tid;

	err = pthread_create(&tid, NULL, thread_func, NULL);
	assert(!err);

	print_id("Main Thread: ");

	int *retval;
	pthread_join(tid, (void *)&retval);
	printf("retval = %d\n", *((int *)&retval));

	return 0;
}
