#include<linux/sem.h>
#include<unistd.h>
#include<linux/tty.h>
#include<linux/kernel.h>
#include<linux/fdreg.h>
#include<asm/system.h>
#include<asm/io.h>
#include<asm/segment.h>
// #include<string.h>  // avoid unknowned error

int cnt;   // the number of semaphores in sem_table, auto initial to zero

sem_t * sys_sem_open(const char *name, unsigned int value){
	int i;
	int name_len = 0;
	int isExist = 0;
	int sem_name_len =0;
	char kernel_sem_name[25]={"\0"};
    char c;
	sem_t * p = NULL;

	while( (c = get_fs_byte(name+name_len))!='\0')
    { 
        kernel_sem_name[name_len] = c;
        ++name_len;
    }
   // printk("name_len: %d\tstrlen: %d\n",name_len,strlen(kernel_sem_name));

	if(name_len > SEM_NAME_LEN) return NULL;

	for(i=0; i<cnt; ++i) /*find duplicate*/
	{
		sem_name_len = strlen(sem_table[i].name);
       // printk("%d:%s!\t%d:%s!\n",sem_name_len,sem_table[cnt].name,name_len,kernel_sem_name);
		if(sem_name_len == name_len && !strcmp(kernel_sem_name, sem_table[i].name))
		{
			isExist = 1;
			break;
		}
	}

	if(isExist) 
	{
       //printk("i am consumer!\n");
		p = sem_table+i;
	}
	else
	{
        strcpy(sem_table[cnt].name, kernel_sem_name);

       // printk("ID:(%d)\t %d\t%s!\t%d\t%s!\n",cnt,strlen(sem_table[cnt].name),sem_table[cnt].name,strlen(kernel_sem_name),kernel_sem_name);

		sem_table[cnt].value = value;
		p = sem_table+cnt;
		++cnt;
        //printk("i am producer!\n");
	}

	return p;
}


void sys_sem_wait(sem_t *sem)
{
	cli();
	if(--sem->value < 0 ) sleep_on(&(sem->queue));
	sti();
}

void sys_sem_post(sem_t *sem)
{
	cli();
	if(++sem->value <= 0) wake_up(&(sem->queue));
	sti();
}

int sys_sem_unlink(const char *name)
{
	int i=0;
	int name_len = 0;
	while(get_fs_byte(name+name_len)!='\0') {++name_len;}
	if(name_len > SEM_NAME_LEN)
		return NULL;

	char kernel_sem_name[25];
	for(i=0; i<name_len; ++i) {kernel_sem_name[i] = get_fs_byte(name+i);}
	
	int isExist = 0;
	int sem_name_len =0;

	for(i =0; i<cnt; ++i)
	{
		sem_name_len = strlen(sem_table[i].name);
		if(sem_name_len == name_len && !strcmp(kernel_sem_name, sem_table[i].name))
		{
			isExist = 1;
			break;
		}
	}

	if(isExist)
	{
		int tmp =0;
		for(tmp=i;i<cnt;++i)
		{
			sem_table[tmp] = sem_table[tmp+1]; // overwrite
		}
		--cnt;
		return 0;
	}	
	else 
		return -1;
}
