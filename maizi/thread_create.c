/*####################################################
# File Name: thread_create.c
# Author: YanWei
# Email: nameqiaohe@126.com
# Create Time: 2016-05-04 23:11:50
# Last Modified: 2016-05-04 23:11:50
####################################################*/
//注意：主线程结束时，所有的子线程也就结束了

#include "apue.h"

struct student{
	int age;
	char name[20];
};

void *thread_func(void *stu){
	printf("student age = %d, name is : %s\n", ((struct student *)stu)->age, ((struct student *)stu)->name);

	return (void *)0;
}

int main(int argc, char *argv[]){
	pthread_t tid;
	int err;

	struct student stu;
	stu.age = 23;
	memcpy(stu.name, "zhangsan", strlen("zhangsan"));

	//第四个参数，是传入给 thread_func函数的参数
	err = pthread_create(&tid, NULL, thread_func, (void *)(&stu));
	assert(!err);

	printf("Main Thread have %d args\n", argc);

	int i = 0;
	for(i; i < argc; i++){
		printf("Main Thread argv[%d] is : %s\n", i, argv[i]);
	}

	pthread_join(tid, NULL);

	return 0;
}
