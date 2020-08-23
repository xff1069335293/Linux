#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>

void sigcb(int signo)
{
    printf("recv a signal : %d\n",signo);
}
int main()
{
    signal(SIGINT,sigcb);   //非可靠信号,无论发多少次，只注册一次
    signal(SIGRTMIN+4,sigcb);//可靠信号

    sigset_t set;       
    sigemptyset(&set);  //清空set   
    sigfillset(&set);   //将所有信号添加到set集合里

    sigprocmask(SIG_BLOCK,&set,NULL);//将set集合中的信号添加到内核中的block阻塞信号集合里

    printf("press enter continue\n");
    getchar();
    
    sigprocmask(SIG_UNBLOCK,&set,NULL);//解除set集合的信号阻塞

    while(1)
    {
        sleep(1);
    }
    return 0;
}
