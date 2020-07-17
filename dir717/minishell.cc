#include<stdio.h>//printf
#include<sys/wait.h>//wait
#include<string.h>//strlen
#include<unistd.h>//exec/fork
#include<stdlib.h>
int main()
{
    while (1)
    {
        printf("[xff@minishell]$ ");
        fflush(stdout);
        //1.wait put
        char buf[1024]={0};
        fgets(buf,1023,stdin);
        buf[strlen(buf)-1]='\0';
        //2.read
        char *argv[32]={NULL};
        int argc=0;
        char *ptr = buf;
        while (*ptr!='\0')
        {
            if (*ptr!=' ')
            {
                argv[argc]=ptr;
                ++argc;
                while (*ptr!=' ' && *ptr!='\0')
                {
                    ++ptr;
                }
                *ptr='\0';
            }
            ++ptr;
        }
        argv[argc]=NULL;
        //3.fork && process replace
        pid_t pid = fork();
        if (pid==0)
        {
            execvp(argv[0],argv);
            perror("execvp error");
            exit(0);
        }
        //4.wait child pid
        waitpid(pid,NULL,0);
    }
    return 0;
}
