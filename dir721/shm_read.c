#include<stdio.h>
#include<sys/shm.h>
#include<string.h>
#include<unistd.h>
#define IPC_KEY 0x12345678
int main()
{
    //1.创建共享内存
    //shmget(key_t key,size_t size, int shmflg)
    int shm_id = shmget(IPC_KEY,32,IPC_CREAT|0664);
    if (shm_id<0){
        perror("shmget error");
        return -1;
    }
    //2.建立映射关系
    //void* shmat(int chmid, const*char pathname,int proj_id)
    void* shm_start = shmat(shm_id, NULL,0);
    if (shm_start==(void*)-1){
        perror("shmat error");
        return -1;
    }
    //3.进行内存操作
    while (1){
        sleep(1);
        printf("%s\n",shm_start);
    }
    //4.解除映射关系
    shmdt(shm_start);
    //5.删除共享内存
    shmctl(shm_id, IPC_RMID,NULL);
    return 0;
}
