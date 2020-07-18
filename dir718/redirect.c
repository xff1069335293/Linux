#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
int main()
{
    //close(1);
    int  fd = open("./test1.txt",O_RDWR|O_CREAT,0664);
    if (fd<0)
    {
        perror("open error");
        return -1;
    }
    dup2(fd,1);
    printf("fd=%d\n",fd);
    //fflush(stdout);
    close(fd);
    return 0;
}
