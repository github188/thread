/*####################################################
# File Name: threadpool.c
# Author: YanWei
# Email: nameqiaohe@126.com
# Create Time: 2016-05-26 05:59:16
# Last Modified: 2016-05-26 05:59:16
####################################################*/

#include "threadpool.h"

#define DEBUG 1

void threadpool_init(threadpool *pool, int max_thread_size){
	pthread_mutex_init(&(pool->mutex), NULL);//init mutex
	pthread_cond_init(&(pool->cond), NULL);//init condition

	pool->runner_head = NULL;//init head and tail, set the value is NULL, 任务队列为空
	pool->runner_tail = NULL;

	pool->max_thread_size = max_thread_size;
	pool->shutdown = 0;

	//创建 分离态线程 ===> 创建线程池
	pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * max_thread_size);
	int index = 0;
	for(index; index < max_thread_size; index++){
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

		pthread_create(&(pool->threads[index]), &attr, (void *)run, (void *)pool);
	}
#if DEBUG
	printf("threadpool_init ===> create %d detached thread\n", max_thread_size);
#endif
}

//线程体
void run(void *arg){
	threadpool *pool = (threadpool *)arg;
	while(1){
		pthread_mutex_lock(&(pool->mutex));// get mutex ===> lock
#if DEBUG
		printf("run ===> locked\n");
#endif
		//若 等待队列为空，并且线程池未销毁，则阻塞，等待任务的唤醒
		while(pool->runner_head == NULL && !pool->shutdown){
			pthread_cond_wait(&(pool->cond), &(pool->mutex));
		}

		if(pool->shutdown){//若线程池已销毁
			pthread_mutex_unlock(&(pool->mutex));// release mutex ===> unlock
#if DEBUG
			printf("run ===> unlocked and thread exit\n");
#endif
			pthread_exit(NULL);
		}

		thread_runner *runner = pool->runner_head;// 取出 链表中的头元素
		pool->runner_head = runner->next;// 更新 链表中的头元素

		pthread_mutex_unlock(&(pool->mutex));
#if DEBUG
		printf("run ===> unlocked\n");
#endif
		(runner->callback)(runner->arg);//调用 回调函数，执行任务
		free(runner);
		runner = NULL;
#if DEBUG
		printf("run ===> runned and free runner\n");
#endif
	}
	pthread_exit(NULL);
}

void threadpool_add_runner(threadpool *pool, void (*callback)(void *arg), void *arg){
	//构造一个新任务
	thread_runner *newrunner = (thread_runner *)malloc(sizeof(thread_runner));
	newrunner->callback = callback;
	newrunner->arg = arg;
	newrunner->next = NULL;

	pthread_mutex_lock(&(pool->mutex));//加锁
#if DEBUG
	printf("threadpool_add_runner ===> locked\n");
#endif
	//将任务 加入到 等待队列中
	if(pool->runner_head != NULL){
		pool->runner_tail->next = newrunner;
		//尾指针 指向 最后一个任务
		pool->runner_tail = newrunner;
	}else{
		pool->runner_head = newrunner;
		pool->runner_tail = newrunner;
	}
	pthread_mutex_unlock(&(pool->mutex));//解锁
#if DEBUG
	printf("threadpool_add_runner ===> unlock\n");
#endif
	//唤醒 一个等待线程
	pthread_cond_signal(&(pool->cond));

#if DEBUG
	printf("threadpool_add_runner ===> add a runner, and weak up a waitting thread\n");
#endif
}

void threadpool_destroy(threadpool **ppool){
	threadpool *pool = *ppool;
	//避免 重复销毁
	if(!pool->shutdown){
		pool->shutdown = 1;
		pthread_cond_broadcast(&(pool->cond));//唤醒所有等待线程，线程池 要销毁了
		sleep(1);//等待所有线程 终止

#if DEBUG
		printf("threadpool_destroy ===> weak up all waitting threads\n");
#endif
		free(pool->threads);//回收空间
		//销毁等待队列
		thread_runner *head = NULL;
		while(pool->runner_head != NULL){
			head = pool->runner_head;//取出 头指针 (取出等待队列中的 第一个任务)
			pool->runner_head = head->next;//更新 头指针 (更新 等待队列中的第一个任务)
			free(head);
		}
#if DEBUG
		printf("threadpool_destroy ===> all runners free\n");
#endif
		//销毁 条件变量 和 互斥量
		pthread_mutex_destroy(&(pool->mutex));
		pthread_cond_destroy(&(pool->cond));
#if DEBUG
		printf("threadpool_destroy ===> mutex and cond destroyed\n");
#endif
		free(pool);
		*ppool = NULL;
#if DEBUG
		printf("threadpool_destroy ===> pool freed\n");
#endif
	}
}
