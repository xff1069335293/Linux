/*================================================================
 *   Copyright (C) 2020 Sangfor Ltd. All rights reserved.
 *   
 *   文件名称：productor_customer.cpp
 *   创 建 者：caofeng
 *   创建日期：2020年09月05日
 *   描    述：生产者消费者模型
 *
 ================================================================*/

#include<unistd.h>
#include<iostream>
#include<queue>
#include<pthread.h>
#include<cstdio>

#define QUEUE_MAX 5
using namespace std;
class BlockQueue
{
    public:
        BlockQueue(int maxq = QUEUE_MAX){
            _capacity = maxq;
            pthread_mutex_init(&_mutex,NULL);
            pthread_cond_init(&_pro_cond,NULL);
            pthread_cond_init(&_cus_cond,NULL);
        }
        ~BlockQueue(){
            pthread_mutex_destroy(&_mutex);
            pthread_cond_destroy(&_pro_cond);
            pthread_cond_destroy(&_cus_cond);
        }
        bool Push(int data){
            //生产者入队数据
            pthread_mutex_lock(&_mutex);
            while (_queue.size() == _capacity)
            {
                pthread_cond_wait(&_pro_cond,&_mutex);
            }
            _queue.push(data);
            pthread_mutex_unlock(&_mutex);
            pthread_cond_signal(&_cus_cond);
            return true;
        }
        bool Pop(int *data){
            pthread_mutex_lock(&_mutex);
            while (_queue.empty())
            {
                pthread_cond_wait(&_cus_cond,&_mutex);
            }
            *data = _queue.front();
            _queue.pop();
            pthread_mutex_unlock(&_mutex);
            pthread_cond_signal(&_pro_cond);
            return true;
        }
    private:
        queue<int> _queue;
        int _capacity;

        pthread_cond_t _pro_cond;
        pthread_cond_t _cus_cond;
        pthread_mutex_t _mutex;
};


void* thr_pro(void *arg)
{
    BlockQueue* queue = (BlockQueue*)arg;
    int i=0;
    while (1)
    {
        //生产者不断生产数据
        queue->Push(i);
        printf("pro push data : %d\n",i++);
    }
    return NULL;
}

void* thr_cus(void *arg)
{
    BlockQueue* queue = (BlockQueue*)arg;
    while (1)
    {
        //消费者不断读取数据
        int data;
        queue->Pop(&data);
        printf("cus pop data : %d\n",data);
    }
}

int main()
{
    int i=0,ret = 0;
    pthread_t ptid[4],ctid[4];

    BlockQueue queue;
    for (i=0; i<4; ++i)
    {
        ret = pthread_create(&ptid[i],NULL,thr_pro,(void*)&queue);
        if (ret != 0)
        {
            printf("pro pthread_create failed\n");
            return -1;  
        }
        ret = pthread_create(&ctid[i],NULL,thr_cus,(void*)&queue);
        if (ret != 0) 
        {
            printf("cus pthread_create failed\n");
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
