#include<stdio.h>
#include<fcntl.h>
#include<sys/types.h>//open
#include<string.h>
#include<unistd.h>//close/write
int main()
{
    //1.int open(const char* pathname,int flag,mode_t mode)
    int fd = open("./test.txt",O_RDWR|O_CREAT,0777);
    if (fd<0)
    {
        perror("open error");
        return -1;
    }
    //2.off_t lseek(int fd,off_t offset,int where)
    lseek(fd,0,SEEK_END);
    //3.ssize_t write(int fd, const void* msg, size_t count)
    ssize_t ret=-1;
    const char* msg = "nihao\n";
    ret = write(fd,msg,strlen(msg));
    if (ret==-1)
    {
        printf("write error\n");
        return -1;
    }
    //4.ssize_t read(int fd,void* buf, size_t count)
    char buf[1024]={0};
    lseek(fd,0,SEEK_SET);
    ret = read(fd,buf,1023);
    if(ret==-1)
    {
        printf("write error\n");
        return -1;
    }
    else if (ret==0)
    {
        printf("empty\n");
    }
    printf("%s\n",buf);
    //5.int close(int fd)
    ret = close(fd);
    if (ret==-1)
    {
        printf("close error\n");
        return -1;
    }
    return 0;
}
