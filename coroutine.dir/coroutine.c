#include "coroutine.h"

//初始化协程管理器
schedule_t* schedule_create()
{
    schedule_t* s = (schedule_t*)malloc(sizeof(schedule_t));
    if (s != NULL)
    {
        s->coroutines = (coroutine_t**)malloc(sizeof(coroutine_t*)*CORSZ); //初始化
        memset(s->coroutines,0x00,sizeof(coroutine_t*)*CORSZ);
        s->current_id = -1;
        s->max_id = 0;
    }
    return s;
}


static void* main_fun(schedule_t* s)
{
    int id = s->current_id;
    if (id != -1)
    { 
        coroutine_t* c = s->coroutines[s->current_id];
        c->call_back(s,c->argc);
        c->state = DEAD;   //目标函数执行完毕
        s->current_id = -1;
    }   
}

//创建协程
int coroutine_create(schedule_t* s, void* (*call_back)(schedule_t*, void* argc),void* argc)
{
    coroutine_t* c =NULL;
    int i=0;
    for (i=0; i<s->max_id; ++i)
    {
        c = s->coroutines[i];
        if (c->state == DEAD)       //如果前面有DEAD状态的，可以复用这个空间
        {
            break;
        }
    }
    if (i == s->max_id || c == NULL)//要新开辟协程结点
    {
        s->coroutines[i] = (coroutine_t* )malloc(sizeof(coroutine_t));
        s->max_id++;
    }
    c = s->coroutines[i];           //设置新协程状态
    c->call_back = call_back;
    c->argc = argc;
    c->state = READY;

    getcontext(&c->ctx);            //保存当前上下文信息
    c->ctx.uc_stack.ss_sp = c->stack;//预分配栈空间
    c->ctx.uc_stack.ss_size = STACKSZ;
    c->ctx.uc_stack.ss_flags = 0;
    c->ctx.uc_link = &s->ctx_main;//后继上下文main
    makecontext(&c->ctx,(void(*)())&main_fun,1,s);//当前c->ctx的上下文环境 main_fun

    return i;
}

//获取协程状态
static enum States get_state(schedule_t* s, int id)
{
    coroutine_t* c = s->coroutines[id];
    if (c == NULL)
        return DEAD;
    return c->state;
}

//启动协程
int coroutine_running(schedule_t* s, int id)
{
    int st = get_state(s,id); 
    if (st == DEAD)
        return ;
     coroutine_t* c = s->coroutines[id];
     c->state = RUNNING;
     s->current_id = id;
    swapcontext(&s->ctx_main,&c->ctx);//保存主线程的上下文信息，切换到当前协程保存的上下文
    return s->current_id;
}




//让出CPU
void coroutine_yield(schedule_t* s)
{
    if (s->current_id != -1)//有协程运行
    {
        coroutine_t* c = s->coroutines[s->current_id];
        c->state = SUSPEND;//设置成挂起状态
        s->current_id = -1;
        swapcontext(&c->ctx,&s->ctx_main);//保存当前协程的上下文信息，切换到主函数上下文运行
    }
}

//还原CPU
void coroutine_resume(schedule_t* s,int id)
{
    coroutine_t* c = s->coroutines[id];
    if (c != NULL && c->state == SUSPEND)//存在协程，并且状态为挂起状态
    {
        c->state = RUNNING;
        s->current_id = id;
        swapcontext(&s->ctx_main,&c->ctx);//将主函数上下文保存，切换到当前协程保存的上下文运行
    }
}

//删除协程
static void delete_coroutine(schedule_t* s,int id)
{
    coroutine_t* c = s->coroutines[id];
    if (c != NULL)
    {
        free(c);
        s->coroutines[id] = NULL;
    }
}

//释放调度器
void schedule_destroy(schedule_t* s)
{
    int i=0;
    for (i=0; i<s->max_id; ++i)//将所有协程空间释放
    {
        delete_coroutine(s,i);
    }
    free(s->coroutines);
   // s->coroutines = NULL;
    free(s);
    //s = NULL;
}

//判断协程是否运行完毕
int schedule_finished(schedule_t* s)
{
    if (s->current_id != -1)//未运行完毕
        return 0;
    int i=0;
    for (i=0; i<s->max_id; ++i)
    {
        coroutine_t* c = s->coroutines[i];
        if (c->state != DEAD)//未运行完毕
            return 0;
    }
    return 1;
}
