#include<unistd.h>
#include<stdio.h>
#include<sys/wait.h>
void main(int argc, char* argv[])
{
	if(!fork())
		printf("I am son!\n");
	else
	{		
		wait(NULL);
		printf("I am father!\n");
	}
}
