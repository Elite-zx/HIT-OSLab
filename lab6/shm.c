#define __LIBRARY__
#include <unistd.h>
#include <linux/kernel.h>
#include <linux/sched.h> /*for current define*/
#include <linux/shm.h>
#include <linux/mm.h> /* PAGE_SIZE,get_free_page */
#include <errno.h>

/*function: apply for a shared page, return shm_id according to key*/
int sys_shmget(key_t key, size_t size)
{
        int i;
        void* page;
      /*printk("hello, i am here for debug!\n");*/
        if(size > PAGE_SIZE) /*size beyond border*/
        {
                printk("size beyond PAGE_SIZE!\n");
                errno = EINVAL;
                return -1;
        }

        for (i=0; i<SHM_SIZE; ++i) 
        {
                if(shm_list[i].key == key)   /*constructed before*/
                {
                        printk("constructed before!\n");
                        return i;
                }
        }

        page = get_free_page();
        if(!page)   /*no free memory*/
        {
                printk("no free page!\n");
                errno = ENOMEM;
                return -1;
        }
        printk("shmget get memory's address is 0x%08x\n",page);
        
        for(i=0; i<SHM_SIZE; ++i)
        {
                if (!shm_list[i].key) // record for manage
                {
                        shm_list[i].key = key; 
                        shm_list[i].page = page;
                        shm_list[i].size = size;
                        return i;
                }
        }
        
        return -1;  /*shm_list is full and key is invalid*/
        
}

void* sys_shmat(int shmid)
{
        unsigned long data_base;
        unsigned long brk;

        if(shmid<0 || shmid>=SHM_SIZE || shm_list[shmid].page == 0)
        {
                errno = EINVAL;
                return (void*)-1;
        }
        data_base = get_base(current->ldt[2]); /*I know get_base from copy_mem() function*/
        printk("current data_base = 0x%08x, new page = 0x%08x\n",data_base,shm_list[shmid].page);

        /* brk is the end of heap section,
         * the virtual memory space between heap and stack can map to shared physical page,
         * so slice this part of virtual memory to map the shared page */
        /*logic address convert to virtual address by adding base address*/
        brk = current->brk+data_base;
        current->brk +=PAGE_SIZE;
        put_page(shm_list[shmid].page, brk);

        return (void*)(current->brk - PAGE_SIZE); /*logic address*/
}
