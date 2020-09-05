/*================================================================
*   Copyright (C) 2020 Sangfor Ltd. All rights reserved.
*   
*   文件名称：RingQueue.cpp
*   创 建 者：caofeng
*   创建日期：2020年09月05日
*   描    述：通过信号量实现生产者与消费者模型,环形队列
*
================================================================*/

#include<unistd.h>
#include<iostream>
#include<vector>
#include<cstdio>
#include<semaphore.h>
using namespace std;


#define MAX_SIZE 5
class RingQueue
{
    public:
        RingQueue(int maxq = MAX_SIZE)
            :_queue(maxq)
             ,_capacity(maxq)
             ,_step_read(0)
             ,_step_write(0)
    {
        sem_init(&_lock,0,1);//实现互斥锁
        sem_init(&_sem_cus,0,0);//数据空间计数初始化为0
        sem_init(&_sem_pro,0,maxq);//空闲空间计数
    }
        ~RingQueue()
        {
            sem_destroy(&_lock);
            sem_destroy(&_sem_pro);
            sem_destroy(&_sem_cus);
        }
        bool Push(int data)
        {
            //1.判断是否能访问资源，不能访问就阻塞
            sem_wait(&_sem_pro);
            //2.能访问加锁,保护访问过程
            sem_wait(&_lock);
            //3.资源的访问
            _queue[_step_write] = data;
            _step_write = (_step_write+1)%_capacity;
            //4.解锁
            sem_post(&_lock);
            //5.入队数据之后，数据空间计数+1，唤醒消费者
            sem_post(&_sem_cus);
            return true;
        }
        bool Pop(int *data)
        {
            //1.判断能否访问资源
            sem_wait(&_sem_cus);
            //2.能访问加锁，保护访问过程
            sem_wait(&_lock);
            //3.资源的访问
            *data = _queue[_step_read];
            _step_read = ( _step_read+1)%_capacity;
            //4.解锁
            sem_post(&_lock);
            //5.出队数据之后，计数空间-1，唤醒生产者
            sem_post(&_sem_pro); 
            return true;
        }
    private:
        int _step_read;//获取数据位置下标
        int _step_write;//写入数据位置下标
        sem_t _lock;
        sem_t _sem_pro;
        sem_t _sem_cus;
        int _capacity;
        vector<int> _queue;

};
void* thr_pro(void* arg)
{
    RingQueue* queue = (RingQueue*)arg;
    int i=0;
    while(1)
    {
        queue->Push(i);
        printf("pro %d\n",i++);    

    }
    return NULL;
}

void* thr_cus(void* arg)
{
    RingQueue* queue = (RingQueue*)arg;
    while(1)
    {
        int data;
        queue->Pop(&data);
        printf("cus %d\n",data);    

    }
    return NULL;
}

int main()
{

    int i=0,ret = 0;
    RingQueue queue;

    pthread_t ptid[4],ctid[4];
    for (i=0; i<4; ++i)
    {
        ret = pthread_create(&ptid[i],NULL,thr_pro,(void*)&queue);
        if (ret != 0)
        {
            printf("pro failed \n");
            return -1;
        }
        ret = pthread_create(&ctid[i],NULL,thr_cus,(void*)&queue);
        if (ret != 0)
        {
            printf("cus failed \n");
            return -1;
        }
    }
    for (i=0; i<4; ++i)
    {
        pthread_join(ptid[i],NULL);
        pthread_join(ctid[i],NULL);

    }
    return 0;
}

