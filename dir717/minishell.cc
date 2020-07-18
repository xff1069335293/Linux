#include<stdio.h>//printf
#include<sys/wait.h>//wait
#include<string.h>//strlen
#include<unistd.h>//exec/fork
#include<stdlib.h>
#include<fcntl.h>
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
        char *argv[32]={NULL};
        int argc=0;
        char *ptr = buf;
        char *data =NULL;
        int file_flag=0;
        //1.5redirct
        while (*ptr!='\0')
        { 
            if (*ptr=='>')
            {
                file_flag=1;
                *ptr='\0';
                ++ptr;
                if (*ptr=='>')
                {
                    file_flag=2;
                    ++ptr;
                }
                while (*ptr!='\0' && *ptr==' ')
                    ++ptr;
                if (*ptr!='\0')
                {
                 data = ptr;
                  while (*ptr!='\0' && *ptr!=' ')
                      ++ptr;
                  *ptr='\0';
                }
            }
            ++ptr;
        }
        //2.read
        ptr=buf;
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
            if (file_flag==1)
            {
                int fd = open(data,O_WRONLY|O_TRUNC|O_CREAT,0664);
                dup2(fd,1);
                close(fd);
            }
            if (file_flag==2)
            {
                int fd = open(data,O_WRONLY|O_APPEND|O_CREAT,0664);
                dup2(fd,1);
                close(fd);
            }
            execvp(argv[0],argv);
            perror("execvp error");
            exit(0);
        }
        //4.wait child pid
        waitpid(pid,NULL,0);
    }
    return 0;
}
