#define __LIBRARY__
#include<unistd.h>
#include<linux/sem.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<linux/sched.h>

_syscall2(sem_t*, sem_open, const char*, name, unsigned int, value)
_syscall1(void, sem_wait, sem_t*, sem)
_syscall1(void, sem_post, sem_t*, sem)
_syscall1(int, sem_unlink, const char*, name)

_syscall2(int, shmget, unsigned int, key, size_t, size)
_syscall1(void*, shmat, int, shmid)

const int NR_PRODUCTS = 50; 
const int BUFFER_SIZE = 10;
const int SHM_KEY = 2023;

int main(int argc, char* argv[])
{
    int shm_id;
    int* ptr;
    int used_cnt = 0; /*products count*/
    int buffer_pos = 0;
    sem_t *mutex, *full, *empty;
	mutex = sem_open("MUTEX",1);
	full = sem_open("FULL",0);
	empty = sem_open("EMPTY",BUFFER_SIZE);
    
    shm_id = shmget(SHM_KEY,BUFFER_SIZE*sizeof(int));
    if (shm_id == -1) 
    {
        printf("shmget failed!");
        return -1;
    }

    if((ptr = (int*)shmat(shm_id)) == (void*)-1)
    {
        printf("shmat failed!");
        return -1;
    }
    
    while(1)
    {
        sem_wait(full);
        sem_wait(mutex);

        printf("pid %d consumer consumes products %d\n",getpid(),ptr[buffer_pos]);
        fflush(stdout);

        sem_post(mutex);
        sem_post(empty);

        buffer_pos = (buffer_pos+1)%BUFFER_SIZE;  /*end of the buffer,refresh*/
        if(++used_cnt == NR_PRODUCTS) break;
    }

    sem_unlink("EMPTY");
    sem_unlink("MUTEX");
    sem_unlink("FULL");
    return 0;
}
