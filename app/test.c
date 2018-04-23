#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>


void led_all_off();
int main(int argc, char **argv) 
{
	int i=9;
	char send[20];
	
	int fd = open("/dev/led_dev_file",O_RDWR);
	
//	read(fd,reciver,i);
//	reciver[i]='\0';
//	printf("%s\n",reciver);
		
//	if(argc!=2)
//	{
//		printf("open test error\n");
//		return 0;
//	}
	
//	send[0]=argv[1][0]-48;
//	send[1]=argv[1][1]-48;
	
//	printf("%d\n",send[0]);
	
	led_all_off(fd);	
	send[0]=0;
	send[1]=1;
	while(1)
	{
		led_all_off(fd);
		write(fd,send,2);
		send[0]=(++send[0])%4;
		sleep(1);
	}
	
	
	close(fd);
	
		
	return 0;
}

void led_all_off(int fd)
{
	write(fd,"\0\0",2);
	write(fd,"\1\0",2);
	write(fd,"\2\0",2);
	write(fd,"\3\0",2);
}

