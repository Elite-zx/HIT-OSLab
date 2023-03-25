#define __LIBRARY__
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
_syscall2(int, whoami, char*, name, unsigned int, size);

int main()
{	
	char username[25] = {0};
	whoami(username, 23);
	printf("username: %s\n", username);
}
