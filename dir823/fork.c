#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<signal.h>
#include<stdlib.h>
void sigcb(int signo)
{
    //循环非阻塞等待，一次性将可以处理的僵尸进程全部处理
    //waitpid >0 等到了 <0 出错  = 没有子进程退出
    int pid;
    while ( pid = waitpid(-1,NULL,WNOHANG)>0){
        printf("child process exit %d\n",pid);
    }
    printf("all child  process exit");
}



int main()
{
    signal(SIGCHLD,sigcb);//SIGCHLD为非可靠信号，多个子进程同时退出，也会产生僵尸进程，所以回调函数要处理
    pid_t pid = fork();
    if (pid == 0)
    {
        printf("child running\n");
        sleep(5);
        exit(66);
    }
    pid = fork();
    if (pid == 0)
    {
        printf("child running\n");
        sleep(10);
        exit(66);
    }

    while (1)
    {
        sleep(1);
        printf("parent running\n");
    }
    return 0;
}
