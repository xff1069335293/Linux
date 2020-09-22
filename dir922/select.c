#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
int main() 
{
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(0, &read_fds);
    for (;;) 
    {
        printf("> ");
        fflush(stdout);
        int ret = select(1, &read_fds, NULL, NULL, NULL);
        if (ret < 0) 
        {
            perror("select");
            continue;
        } 
        if (FD_ISSET(0, &read_fds)) 
        {
            char buf[1024] = {0};
            read(0, buf, sizeof(buf) - 1);
            printf("input: %s", buf);
        } 
        else 
        {
            printf("error! invaild fd\n");
            continue;
        }
        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
    }
    return 0;
}
