#include<string.h>
#include<asm/segment.h>
#include<errno.h>

char str_pos[24];
int sys_iam(const char* name)
{
	char c ;
	int i = 0;
	while((c = get_fs_byte(name+i)) != '\0')
	{
		str_pos[i] = c;
		++i;
	}
	
	if(i > 23)
	{
		errno = EINVAL;
		return -1;
	}
        printk("elitezx lab2 string:  %s\n",str_pos );	
	return i;
}

int sys_whoami(char* name, unsigned int size)
{
	if(size<strlen(str_pos))
	{
		errno = EINVAL;
		return -1;
	}
	int ans = 0;
	char c;
	while((c = str_pos[ans] )!='\0')
	{
		put_fs_byte(c,name++);
		++ans;
	}
	return ans;
}
