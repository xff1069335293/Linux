#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
int a=100;
void *p_start(void*argc)
{
    a = 10;
    while(1)
    {
        printf("i am a thread :%s a = %d\n",argc,a);
        sleep(1);
    }
    return NULL;
}
int main()
{
    char *argc="chifanlema\n";
    pthread_t tid;
    // pthread_create(获取线程id,设置线程属性，入口函数，参数)
    int ret = pthread_create(&pid,NULL,p_start,argc);
    if (ret != 0)
    {
        printf("create thread failed \n");
        return -1;
    }
    printf("create thread success \n");

    while(1)
    {
        printf("i am  main : nihao a = %d!!\n",a);
        sleep(1);
    }
    return 0;
}
