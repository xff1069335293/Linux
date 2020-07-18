#include<stdio.h>
#include<unistd.h>
int main()
{

    fwrite("fwrite",1,6,stdout);
    write(1,"write",5);
    sleep(3);
    return 0;
    }

