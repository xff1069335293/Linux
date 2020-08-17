#include"coroutine.h"

//获取监听描述符
int tcp_init()
{
    printf(" tcp_init()\n");
    //创建套接字
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    if (lfd == -1)
    {
        perror("socket");
        exit(1);
    }
    //设置套接字复用
    int op = 1;
    setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,&op,sizeof(op));
    //绑定地址信息
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int fd = bind(lfd,(struct sockaddr*)&addr,sizeof(addr));
    if (fd == -1)
    {
        perror("bind");
        exit(1);
    }
    //开始监听
    listen(lfd,SOMAXCONN);

    printf("tcp_init-> lfd = %d\n",lfd);
    return lfd;
}

//设置成非阻塞
void set_nonblock(int fd)
{
    //获取描述符的访问模式和文件状态
    int flgs = fcntl(fd, F_GETFL, 0);
    //设置为非阻塞
    flgs |= O_NONBLOCK;
    fcntl(fd,F_SETFL, flgs);
}

//获取新连接
void accept_conn(int lfd, schedule_t* s, int co_ids[], void*(*call_back)(schedule_t* s, void *argc))
{
    while (1)
    {
        sleep(3);
        int cfd = accept(lfd,NULL, NULL);
        printf("accept_conn-> cfd = %d\n",cfd);
        if (cfd > 0)
        {
            set_nonblock(cfd);//设置为非阻塞
            int argc[] = {lfd,cfd};
            int id = coroutine_create(s,call_back,argc);//创建协程，并且设置好上下文等信息
            int i=0;
            for (i=0 ;i<CORSZ; ++i)
            {
                if (co_ids[i] == -1)
                {
                    co_ids[i] = id;
                    break;
                }
            }
            if (i == CORSZ)
            {
                printf("连接太多\n");
            }
            printf("accept_conn -> coroutine_running\n");
            //启动协程
            coroutine_running(s,id);
        }
        else 
        {
            int i=0; 
            for (i=0; i<CORSZ; ++i)
            {
                int cid = co_ids[i];
                if (cid == -1)
                    continue;//如果没有对应的描述符就一直向后找
                else
                {
                    printf("accept_conn -> coroutine_resume\n");
                    coroutine_resume(s,i);//有对应的描述符，就恢复协程
                }
            }
        }
    }
}

//回调函数
void* handle(schedule_t* s, void* argc)
{
    printf("handle\n");

    char buf[1024] = {};
    int *arr = (int*)argc;
    int cfd = arr[1];
    while (1)
    {
        sleep(3);
        printf("handle while\n");
        memset(buf, 0x00, sizeof(buf));
        int r = read(cfd, buf, 1024);
        printf("read = %d\n",r);
        if (r == -1)
        {
            //读取错误就让出CPU
            printf("handle -> coroutine_yield\n");
            coroutine_yield(s);
        }
        else if (r == 0){
            //读到结尾就结束
            printf("read end\n");
            break;
            }
        else 
        {
            //从cfd中读到buf,并且输出
            printf("recv: %s\n",buf);
            //比较buf前4字节和exit是否完全匹配,收到exit就退出
            if (strncasecmp(buf, "exit",4) == 0)
             {
                printf("exit\n");
                break;
             }
            //将buf里的数据写入cfd中
            write(cfd, buf, r);
        }
    }
}

int main(void)
{
    printf("server start\n");
    int lfd = tcp_init();
    set_nonblock(lfd);//将监听套接字设置为非阻塞

    schedule_t* s = schedule_create();
    int co_ids[CORSZ];//存储已经的客户端的描述符
    int i=0;
    for (i=0 ;i<CORSZ; ++i)
    {
        co_ids[i] = -1;
    }
    accept_conn(lfd,s,co_ids,handle);//连接

    schedule_destroy(s);
}
