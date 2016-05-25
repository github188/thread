/*####################################################
# File Name: threadpool.h
# Author: YanWei
# Email: nameqiaohe@126.com
# Create Time: 2016-05-26 05:47:09
# Last Modified: 2016-05-26 05:47:09
# Description : 线程池 的简单实现
# 	参考博客：http://www.cnblogs.com/xiehongfeng100/p/4663942.html	
####################################################*/

#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include "apue.h"

//线程体 数据结构
typedef struct runner{
	void (*callback)(void *arg);//回调函数指针
	void *arg;//回调函数参数
	struct runner *next;
}thread_runner;

//线程池 数据结构
typedef struct{
	pthread_mutex_t mutex;//互斥锁
	pthread_cond_t cond;//条件变量

	thread_runner *runner_head;//线程池中 所有等待任务的 头指针
	thread_runner *runner_tail;//线程池中 所有等待任务的 尾指针

	int shutdown;//线程池 是否销毁

	pthread_t *threads;//所有线程

	int max_thread_size;//线程池中 允许的 活动线程数目
}threadpool;

//线程体
void run(void *arg);

//初始化线程池
void threadpool_init(threadpool *pool, int max_thread_size);

//向 线程池 添加任务
void threadpool_add_runner(threadpool *pool, void (*callback)(void *arg), void *arg);

//销毁线程池
void threadpool_destroy(threadpool **pool);

#endif// THREADPOOL_H_
