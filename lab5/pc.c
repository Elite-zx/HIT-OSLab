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

const int NR_CONSUMER = 5;
const int NR_PRODUCTS = 50; 
const int BUFFER_SIZE = 10;
unsigned int pro_nr, csm_pro_nr; /*product number, consumed product number*/
const char* FILENAME = "/usr/root/buffer_file"; /*default buffer file*/
sem_t *mutex, *full, *empty;
int f_in;   /* for producer*/
int f_out;  /* for consumer*/
int main(int argc, char* argv[])
{
	const char * file_name = argc > 1 ? argv[1] : FILENAME;
	int pid;
	int cnt; /* must declare cnt here */
/*open buffer file */
	f_in = open(file_name, O_CREAT|O_TRUNC|O_WRONLY, 0222);
	f_out = open(file_name,O_TRUNC|O_RDONLY, 0444);
/*create semaphores*/
	mutex = sem_open("MUTEX",1);
	full = sem_open("FULL",0);
	empty = sem_open("EMPTY",BUFFER_SIZE);
/*parent process as producer*/
	pro_nr = 0;
	if(fork())
	{
		pid = getpid();
		printf("pid %d:\t producer created!\n",pid);
		fflush(stdout);
		while(pro_nr < NR_PRODUCTS) /*until product all products then producer exit*/
		{
			sem_wait(empty);
			sem_wait(mutex);

			if(!(pro_nr % BUFFER_SIZE)) lseek(f_in, 0, 0); /*10 products per turn*/

			write(f_in,(char*)&pro_nr, sizeof(pro_nr));
			printf("pid %d:\t produces product %d!\n", pid, pro_nr);
			fflush(stdout);
			++pro_nr;

			sem_post(mutex);
			sem_post(full);
		}
	}
/*child process create child processes to be consumer*/
	else  
	{
		cnt = NR_CONSUMER;
		while(cnt--)
		{
			if(!(pid=fork()))
			{
				pid = getpid();
				printf("pid %d:\t consumer %d created!\n", pid, NR_CONSUMER-cnt);
				fflush(stdout);

				while(1)
				{
					if(csm_pro_nr == NR_PRODUCTS) goto OK;	
					
					sem_wait(full);
					sem_wait(mutex);
					
					if(!read(f_out,(char*)&csm_pro_nr, sizeof(csm_pro_nr))) /*end of file,reset*/
					{
						lseek(f_out, 0, 0);
						read(f_out,(char*)&csm_pro_nr, sizeof(csm_pro_nr));
					}

					printf("pid:%d:\t consumer %d consume product %d\n",pid, NR_CONSUMER-cnt,csm_pro_nr);
					fflush(stdout);
					sem_post(mutex);
					sem_post(empty);

					
				}
				

			}
		}

	}

OK:	
	sem_unlink("MUTEX");
	sem_unlink("FULL");
	sem_unlink("EMPTY");

	close(f_in);
	close(f_out);
	return 0;
}










