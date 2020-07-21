#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
int main()
{   
   umask(0);
    int ret = mkfifo("hello.fifo",0664);
    if (ret<0&& errno!=EEXIST){
        perror("mkfifo error");
        return -1;
    }
    int fd = open("hello.fifo",O_RDONLY);
    if (fd<0){
        printf("open error");
        return -1;
    }
    printf("open success\n");
    while (1){
    char buf[1024]={0};
    ret = read(fd,buf,1023);
    if (ret<0){
        printf("read error\n");
        return -1;
    }
    else if (ret ==0){
        printf("all write close\n");
        return -1;
        }
    printf("ret=%d buf=%s\n",ret,buf);
    }
    close(fd);
    return 0;
}
