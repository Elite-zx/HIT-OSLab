#ifndef __SHM
#define __SHM


#define SHM_SIZE 32 /*Maximum number of shared pages*/

typedef unsigned int key_t;

typedef struct shm
{
        key_t key;  
        unsigned int size;  
        unsigned long page; /*shared page address */
}shm;

shm shm_list[SHM_SIZE] ={{0,0,0}};


#endif
