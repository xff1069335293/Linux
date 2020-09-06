/*================================================================
*   Copyright (C) 2020 Sangfor Ltd. All rights reserved.
*   
*   文件名称：threadpool.hpp
*   创 建 者：caofeng
*   创建日期：2020年09月06日
*   描    述：实现一个线程池
*
================================================================*/


#include<cstdio>
#include<iostream>
#include<queue>
#include<stdlib.h>
#include<pthread.h>

using namespace std;
typedef void (*handler_t)(int data);


//创建一个任务
class ThreadTask
{
	public:
	void SetTask(int data,handler_t handler)
	{
		_data = data;
		_handler = handler;
	}
	void Run()
	{
		return _handler(_data);  
	}
	private:
	int _data;//任务中需要处理的数据
	handler_t _handler;//任务中需要处理数据的方法
};



#define MAX_THREAD 5
class ThreadPool
{
	public:
	ThreadPool(int thr_max = MAX_THREAD)
	:_thr_max(thr_max)
	{
		pthread_mutex_init(&_mutex,NULL);
		pthread_cond_init(&_cond,NULL);
		int i,ret;
		pthread_t tid[_thr_max];
		for (i=0; i<_thr_max; ++i)
		{
			ret = pthread_create(&tid[i],NULL,thr_start,this);
			if (ret != 0)
			{
				printf("thread create failed\n");
				exit(-1);
			}
		}
	}
	~ThreadPool()
	{
		pthread_mutex_destroy(&_mutex);
		pthread_cond_destroy(&_cond);
	}
	bool Push(ThreadTask &task)
	{
		pthread_mutex_lock(&_mutex);
		_queue.push(task);
		pthread_mutex_unlock(&_mutex);
		pthread_cond_broadcast(&_cond);//唤醒所等待的处理线程
		return true;
	}
	static void *thr_start(void *arg)
	{	
		ThreadPool *p = (ThreadPool *)arg;
		ThreadTask task;
		while (1)
		{
		pthread_mutex_lock(&p->_mutex);
		while (p->_queue.empty())
		{
			pthread_cond_wait(&p->_cond,&p->_mutex);
		}
		task = p->_queue.front();
		p->_queue.pop();
		pthread_mutex_unlock(&p->_mutex);
		task.Run();//任务的处理
		}
		return NULL;
	}
	private:
	int _thr_max;//线程池中最大线程数量
	queue<ThreadTask> _queue;
	pthread_mutex_t _mutex;//保护队列操作的互斥锁
	pthread_cond_t _cond;//实现队列获取节点的同步条件变量
};