/*================================================================
 *   Copyright (C) 2020 Sangfor Ltd. All rights reserved.
 *   
 *   文件名称：mutex.c
 *   创 建 者：caofeng
 *   创建日期：2020年08月26日
 *   描    述：以黄牛抢票为例子，如果多执行访问临界资源，不进行保护有可能造成的问题
 *
 ================================================================*/


#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
//pthread_mutex_t mtx;//创建互斥量
//pthread_mutex_init(&mtx,NULL);//初始化
//pthread_mutex_t mtx=PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_lock(&mtx);//阻塞加锁
//pthread_mutex_trylock(&mtx);//非阻塞加锁，失败返回EBUSY
//pthread_mutex_unlock(&mtx);//解锁
//pthread_mutex_destroy(&mtx);//销毁互斥变量
//互斥变量也是临界资源，原子操作实现互斥
pthread_mutex_t mtx;
int ticket = 100;
void* thr_scalpers()
{
    while(1)
    {
        pthread_mutex_lock(&mtx);
        if (ticket>0)
        {
            printf("tid = %d got a ticket: %d\n",pthread_self(),ticket);
            --ticket;
            usleep(1000);
            pthread_mutex_unlock(&mtx);
        }
        else
        {
            pthread_mutex_unlock(&mtx);
            pthread_exit(NULL);
        }
    }
    return NULL;
}
int main()
{   
    pthread_mutex_init(&mtx,NULL);
    pthread_t tid[4];
    int i;
    for(i = 0; i<4; i++)
    {
        int ret = pthread_create(&tid[i],NULL,thr_scalpers,NULL);
        if (ret != 0)
        {
            printf(" pthread_create failed\n");
            return -1;
        }
    }
    for (i = 0; i<4; i++)
    {
        pthread_join(tid[i],NULL);
    }
    pthread_mutex_destroy(&mtx);
    return 0;
}
