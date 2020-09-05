/*================================================================
*   Copyright (C) 2020 Sangfor Ltd. All rights reserved.
*   
*   文件名称：cond.c
*   创 建 者：caofeng
*   创建日期：2020年08月28日
*   描    述：演示条件变量基本使用流程和注意事项
*
================================================================*/
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>

int bowl = 0;
pthread_cond_t cond;
pthread_mutex_t mutex;

void* thr_cook()
{
 while (1)
 {
     //加锁
     pthread_mutex_lock(&mutex);
    if (bowl != 0)
    {
        sleep(1);
        //有饭,厨师线程等待
        //唤醒顾客线程
        //解锁
        printf("有饭，厨师线程挂起，唤醒顾客线程\n");
        pthread_cond_wait(&cond,&mutex);
    }
    //没有饭，做饭
    //唤醒顾客线程
    //解锁
    sleep(1);
     printf("没有饭，厨师线程做饭\n");
    bowl = 1;
    printf("厨师线程 做饭后-> %d唤醒顾客线程吃饭\n",bowl);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
 }
 return NULL;
}

void* thr_customer()
{
    while (1)
    {
        //加锁
        pthread_mutex_lock(&mutex);
        if (bowl == 0)
        {
            sleep(1);
        printf("没有饭，顾客线程挂起，唤醒厨师线程\n ");
        //pthread_mutex_unlock(&mutex);
        pthread_cond_wait(&cond,&mutex);
        }
        sleep(1);
         printf("有饭，顾客线程吃饭\n ");
        --bowl;
        printf("顾客线程 吃饭后-> %d，唤醒厨师线程做饭\n",bowl);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main()
{

    pthread_t cook_tid,customer_tid;
    int ret =  pthread_create(&cook_tid,NULL,thr_cook,NULL);
    if (ret != 0)
    {
        printf("pthread_create cook_tid failed\n");
        return -1;
    }
    ret =  pthread_create(&customer_tid,NULL,thr_customer,NULL);
    if (ret != 0)
    {
        printf("pthread_create customer_tid failed\n");
        return -1;
    }
    pthread_join(cook_tid,NULL);
    pthread_join(customer_tid,NULL);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return 0;
}

