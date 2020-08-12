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
        if (c->state == DEAD)
        {
            break;
        }
    }
    if (i == s->max_id || c == NULL)
    {
        s->coroutines[i] == (coroutine_t* )malloc(sizeof(coroutine_t));
        s->max_id++;
    }
    c = s->coroutines[i];
    c->call_back = call_back;
    c->argc = argc;
    c->state = READY;
    getcontext(&c->ctx);

    c->ctx.uc_stack.ss_sp = c->stack;
    c->ctx.uc_stack.ss_size = STACKSZ;
    c->ctx.uc_stack.ss_flags = 0;
    c->ctx.uc_link = &s->ctx_main;
    makecontext(&c->ctx,(void(*)())&main_fun,1,s);

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
void coroutine_running(schedule_t* s, int id)
{
    int st = get_state(s,id); 
    if (st == DEAD)
        return ;
     coroutine_t* c = s->coroutines[id];
     c->state = RUNNING;
     s->current_id = id;
    swapcontext(&s->ctx_main,&c->ctx);
    //return s->current_id;
}




//让出CPU
void coroutine_yield(schedule_t* s)
{
    if (s->current_id != -1)
    {
        coroutine_t* c = s->coroutines[s->current_id];
        c->state = SUSPEND;
        s->current_id = -1;
        swapcontext(&c->ctx,&s->ctx_main);
    }
}

//还原
void coroutine_resume(schedule_t* s,int id)
{
    coroutine_t* c = s->coroutines[id];
    if (c != NULL && c->state == SUSPEND)
    {
        c->state = RUNNING;
        s->current_id = id;
        swapcontext(&s->ctx_main,&c->ctx);
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
    for (i=0; i<s->max_id; ++i)
    {
        delete_coroutine(s,i);
    }
    free(s->coroutines);
    //s->coroutines = NULL;
    free(s);
    //s = NULL;
}

//判断协程是否运行完毕
int schedule_finished(schedule_t* s)
{
    if (s->current_id = -1)
        return 0;
    int i=0;
    for (i=0; i<s->max_id; ++i)
    {
        coroutine_t* c = s->coroutines[i];
        if (c->state != DEAD)
            return 0;
    }
    return 1;
}

