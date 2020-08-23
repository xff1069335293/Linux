#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
void sigcb(int signo) 
{

    printf("recv a signal no:%d\n",signo);
}

int main()
{
    //kill(getpid(),SIGHUP);//给指定进程发送指定信号
    //raise(SIGTERM);       //给自己发送指定信号
    //alarm(3); //定时器，三秒之后给自己发送SIGALRM信号
    //abort();  //给自己发送SIGABRT用于异常通知
    signal(SIGINT,sigcb);
    while (1)
    {
        sleep(1);
        printf("晚上好\n");
    }
    return 0;
}
