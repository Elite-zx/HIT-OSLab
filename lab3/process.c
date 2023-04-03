#include<unistd.h>
#include<stdio.h>
#include<time.h>
#include<sys/times.h>
#include<sys/types.h>
#include<sys/wait.h>
void cpuio_bound(int last, int cpu_time, int io_time);


void main(int argc, char* argv[])
{
	pid_t son_proc_pid[21];
	int i = 0 ;
	while(i<21)
	{
		if(! (son_proc_pid[i] = fork()))
		{
			cpuio_bound(20,i,20-i);
			return;
		}
		++i;
	}

	i = 0;
	while(i<21)
	{
		printf("child_process_pid: %d\n", son_proc_pid[i]);
		++i;
	}
	wait(NULL);
}


void cpuio_bound(int last, int cpu_time, int io_time)
{
	struct tms start, pre;
	clock_t sum_cpu_time = 0 ;
	clock_t accumulate =0;

	while(1)
	{
		times(&start);
		while(sum_cpu_time < cpu_time)
		{
			times(&pre);
			sum_cpu_time = (pre.tms_utime - start.tms_utime + pre.tms_stime - pre.tms_stime)/100;
		}

		if(sum_cpu_time>=last) break;  

		sleep(io_time);
		if((accumulate+= io_time + cpu_time)>=last)
			break;
	}	
}
