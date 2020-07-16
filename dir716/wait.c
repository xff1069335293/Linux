#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
int main()
{
    pid_t pid = fork();
    if (pid ==0 )
    {
        sleep(5);
        exit(66);//return val
    }
    //wait(NULL);
    int ret, status;
    //pid_t pid=waitpid(pid,&status,0/WNOHANG)//status=child return val 
    while ((ret=waitpid(pid,&status,WNOHANG))==0)
    {
        printf("child working\n");
        sleep(1);
    }
    if ((status&0x7f)==0)
    {
       printf("child exit status=%d ret=%d\n",(status>>8)&0xff,ret);
    }
   if (WIFEXITED(status))
   {
       printf("status=%d\n",WEXITSTATUS(status));
   }
    while (1)
    {
        printf("working\n");
        sleep(1);
    }
    return 0;
    
}
