#ifndef _SEM
#define _SEM

#include<linux/sched.h>   // for task_struct definition
#define SEM_TABLE_LEN 	20
#define SEM_NAME_LEN	20

typedef struct semaphore{
	int value;
	char name[SEM_NAME_LEN];
	struct task_struct *queue;   // wait queue
}sem_t;

sem_t sem_table[SEM_TABLE_LEN];

#endif

