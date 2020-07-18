#include<stdio.h>
#include<string.h>
int main()
{
    //1.fopen
    //FILE* fp = fopen("hello1.txt","w+");
    FILE* fp = fopen("hello.txt","r+");
    //r-read w-write 
    //r+ read and write w+ read and write
    //a-write and add a+ read and write add
    if (!fp)
    {   
        printf("fopen error\n");
        return -1;
    }
    char buf[1024]={0};
    const char* msg="hello world\n";
    //2.fseek
    fseek(fp,0,SEEK_END);
    //3.fwrite
    int ret1 = fwrite(msg,strlen(msg),1,fp);
    if (ret1==0)
    {
        perror("fwrite error");
        return -1;
    }
    //4.fread
    fseek(fp,0,SEEK_SET);
    size_t ret = fread(buf,1,1023,fp);
    if (ret==0)
    {
        printf("no data\n");
        return -1;
    }
    printf("%u\n%s\n",ret,buf);
    //5.fclose
    fclose(fp);
    return 0;
}

