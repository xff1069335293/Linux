#include "threadpool.hpp"
#include<unistd.h>

void test_func(int data)
{
	int sec = (data % 3)+1;
	printf("tid: %p ---- test_func\n",pthread_self());
	sleep(sec);
}

void tmp_func(int data)
{
	printf("tid: %p ---- tmp_func\n",pthread_self());
	sleep(1);
}


int main()
{
	ThreadPool pool;
	for (int i=0; i<10; ++i)
	{
		ThreadTask task;
		if (i%2)
			task.SetTask(i,tmp_func);
		else
			task.SetTask(i,test_func);
		pool.Push(task);
	}
	printf("finish !\n");
	sleep(20);
	return 0;
}