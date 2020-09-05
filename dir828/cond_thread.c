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
pthread_cond_t cook_cond;
pthread_cond_t customer_cond;
pthread_mutex_t mutex;

void* thr_cook()
{
    while (1)
    {
        //加锁
        pthread_mutex_lock(&mutex);
        //要循环判断，因为pthread_cond_signal唤醒至少一个线程，所以，有可能将多个顾客线程唤醒，那么都去卡到加锁的状态，那么加锁成功的顾客吃完饭解锁，然后唤醒顾客线程，那么此时加锁的线程有顾客线程和厨师线程，如果顾客加锁成功，那么下来吃饭是没有饭的，所以逻辑就错误了，这里循环判断一下，当有顾客线程卡在加锁时，循环判断是否满足条件
        while (bowl != 0)
        {
            sleep(1);
            //有饭,厨师线程等待
            //唤醒顾客线程
            //解锁
            printf("%d 有饭，厨师线程挂起\n",pthread_self());
            pthread_cond_wait(&cook_cond,&mutex);
        }
            //没有饭，做饭
            //唤醒顾客线程
            //解锁
            sleep(1);
            printf("%d 没有饭，厨师线程做饭\n",pthread_self());
            ++bowl;
            printf("%d 厨师线程 做饭后-> %d唤醒顾客线程吃饭\n",pthread_self(),bowl);
            pthread_cond_signal(&customer_cond);
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
        while (bowl == 0)
        {
            sleep(1);
            printf("%d 没有饭，顾客线程挂起\n ",pthread_self());
            //pthread_mutex_unlock(&mutex);
            pthread_cond_wait(&customer_cond,&mutex);
        }
            sleep(1);
            printf("%d 有饭，顾客线程吃饭\n ",pthread_self());
            --bowl;
            printf("%d 顾客线程 吃饭后-> %d，唤醒厨师线程做饭\n",pthread_self(),bowl);
            pthread_cond_signal(&cook_cond);
            pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main()
{

    int i=0;
    pthread_t cook_tid[4],customer_tid[4];
    
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cook_cond,NULL);
    pthread_cond_init(&customer_cond,NULL);
    
    
    for (i=0; i<4; ++i)
    {
        int ret =  pthread_create(&cook_tid[i],NULL,thr_cook,NULL);
        if (ret != 0)
        {
            printf("pthread_create cook_tid failed\n");
            return -1;
        }
        ret =  pthread_create(&customer_tid[i],NULL,thr_customer,NULL);
        if (ret != 0)
        {
            printf("pthread_create customer_tid failed\n");
            return -1;
        }
    }
    pthread_join(cook_tid[0],NULL);
    pthread_join(customer_tid[0],NULL);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cook_cond);
    pthread_cond_destroy(&customer_cond);
    return 0;
}

