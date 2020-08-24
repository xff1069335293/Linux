#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
void func()
{
    //char retval[] 在栈区，函数返回，局部变量就销毁了  
    char* retval="这是我的返回值\n";//这个在常量区，是一个指针
    pthread_exit((void*)retval);
}
void *p_start()
{
    //exit(0);//任意位置退出进程
   /* while(1)
    {
        printf("*p_start()\n");
        //func();
        sleep(5);
    }*/
    sleep(10);
    printf("*p_start()\n");
    //在任意位置都可以,谁调用谁退出，主动退出线程
    /*pthread_exit(NULL);*/
    return NULL;
}

int main()
{
    pthread_t tid;
    int ret = pthread_create(&tid,NULL,p_start,NULL);
    if (ret != 0)
    {
        printf("pthread_create failed\n");
        return -1;
    }
    //线程分离，不关心返回值,可以等待任意线程，只有joinable的线程     //才能被等待
    pthread_detach(tid);
    //pthread_self()获取自己的tid
    /*//阻塞等待
    char *retval;
    pthread_join(tid,(void**)&retval);
    printf("子线程退出: %s\n",retval);
    //pthread_cancel(tid);//终止指定进程*/
    while (1)
    {
        printf("main return\n");
        sleep(1);
    }
    //主线程调用return是退出整个进程，子线程调用return是退出子线程
    return 0;
}
