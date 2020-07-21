#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
int main()
{
    int pipefd[2]={-1};
    int ret = pipe(pipefd);
    if (ret<0){
        perror("pipe error");
        return -1;
    }
    pid_t pid = fork();
    if (pid==0){
        //子进程读
        //close(pipefd[0]);
        close(pipefd[1]);
        sleep(5);
        char buf[1024]={0};
        read(pipefd[0],buf,1023);
        printf("ret = %d buf = %s\n",ret,buf);
    }
    char* msg="hello world";
    while (1){
        close(pipefd[1]);
        //close(pipefd[0]);
        write(pipefd[1],msg,strlen(msg));
        
    sleep(1);
    }
    return 0;
}
