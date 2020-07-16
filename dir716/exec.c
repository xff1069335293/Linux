#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
int main(int argc,char* argv[],char* env[])
{
    printf("nihaoa~~~~\n");
    //execl("./test","test","ls","-l",NULL);
    char *newargv[]={"mytest","ls","-l",NULL};
    char *newenv[]={"MYVAL=1000"};
    //execv("./mytest",newargv);
    //execvp("mytest",newargv);
    execve("./mytest",newargv,newenv);
    return 0;
}
