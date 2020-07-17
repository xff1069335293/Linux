#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
int main()
{
    char buf[1024]={0};
    fgets(buf,1023,stdin);
    buf[strlen(buf)-1]='\0';
    char *ptr =buf;
    char *n_argv[32]={NULL};
    int n_argc=0;
    while (*ptr!='\0')
    {
        if(*ptr!=' ')
        {
             n_argv[n_argc++]=ptr;
            while (*ptr!=' '&& *ptr!='\0')
            {
             ++ptr;
            }
            *ptr='\0';
        }
        ++ptr;
    }
    n_argv[n_argc]=NULL;
    int i=0;
    for (i=0; i<n_argc; ++i)
    {
         printf("n_argv[%d]=%s\n",i,n_argv[i]);
    }
    return 0;
}
