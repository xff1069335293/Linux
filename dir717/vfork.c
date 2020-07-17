#include<stdio.h>
#include<stdlib.h>//exit
#include<unistd.h>//vfork/sleep
int main()
{
    pid_t pid1 = vfork();
   /* if (pid==0)
    {
        printf("child process runing\n");
        sleep(3);
       // return 0; //fflush buffer 
       // exit(0);  //exit current process and fflush buffer
       _exit(0);    //system function exit current process
    }*/
     printf("pid1=%d process runing\n",pid1);
    return 0;
    //exit(0);
    //_exit(0);
}

