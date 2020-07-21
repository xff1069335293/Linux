#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
int main()
{
    int ret = mkfifo("hello.fifo",0664);
    if (ret<0&& errno!=EEXIST){
        perror("mkfifo error");
        return -1;
    }
    int fd = open("hello.fifo",O_WRONLY);
    if (fd<0){
        printf("open error\n");
        return -1;
    }
    printf("open success\n");
    char* msg="hello world";
    while (1){
    write(fd,msg,strlen(msg));
    sleep(1);
    }
    close(fd);
    return 0;
}

