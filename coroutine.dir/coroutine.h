#ifndef __COROUTINE_H__
#define __COROUTINE_H__

#include<stdio.h>
#include<malloc.h>
#include<ucontext.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>

#define CORSZ   1024
#define STACKSZ 1024*64
enum States
{
    DEAD,       //死亡
    READY,      //就绪
    RUNNING,    //运行
    SUSPEND     //阻塞
};
struct schedule;

//协程结构体
typedef struct coroutine{
    void* (*call_back)(struct schedule* s,void* argc); //回调函数
    void* argc;             //回调函数参数
    ucontext_t ctx;         //协程上下文
    char stack[STACKSZ];     //协程栈
    enum States state;     //协程状态
}coroutine_t;

//协程管理器
typedef struct  schedule{
    coroutine_t** coroutines;       //所有协程
    int current_id;     //当前协程id，如果没有正在运行的协程设置为-1
    ucontext_t ctx_main;//主流程上下文
    int  max_id;      //最大下标
}schedule_t;

//初始化协程管理器
schedule_t* schedule_create();

//创建协程
int coroutine_create(schedule_t* s, void* (*call_back)(schedule_t*, void* argc),void* argc);

//启动协程
int coroutine_running(schedule_t* s, int id);

//让出CPU
void coroutine_yield(schedule_t* s);

//还原CPU
void coroutine_resume(schedule_t* s,int id);

//释放调度器
void schedule_destroy(schedule_t* s);

//判断协程是否运行完毕
int schedule_finished(schedule_t* s);


#endif /*__COROUTINE_H__*/
