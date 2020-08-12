#include "coroutine.h"
void* fun1(schedule_t* s,void* argc)
{
    printf("fun1 running\n");
    coroutine_yield(s);
    int val1 = *(int*)argc;
    printf("fun1 exit: %d\n",val1);
    return NULL;
}

void* fun2(schedule_t* s,void* argc)
{
    printf("fun2 running\n");
    coroutine_yield(s);
    int val2 = *(int*)argc;
    printf("fun2 exit: %d\n",val2);
    return NULL;
}

int main()
{
    schedule_t* s = schedule_create();

    int* val1 = (int*)malloc(sizeof(int));
    *val1 = 1;
    int id1 = coroutine_create(s,fun1,val1);


    int* val2 = (int*)malloc(sizeof(int));
    *val2 = 2;
    int id2 = coroutine_create(s,fun2,val2);

    coroutine_running(s,id1);
    coroutine_running(s,id2);

    while ( !schedule_finished(s))
    {
        coroutine_resume(s,id1);
        coroutine_resume(s,id2);
    }

    schedule_destroy(s);
    return 0;
}
