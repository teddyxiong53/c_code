#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <mqueue.h>
#include "msg.h"

#define BUFFER 8192 

int main()
{
	mqd_t id;
	struct MsgType msg;
	struct mq_attr attr;
	const char *file = "/posix";
	int ret = 0;
	unsigned int sender = 0;
	unsigned int recv_size = BUFFER;
	
	id = mq_open(file, O_RDWR);
	if(id < 0)
	{
		perror("mq_open");
		exit(1);
	}
	ret = mq_getattr(id, &attr);
	if(ret < 0)
	{
		perror("mq_getattr");
		exit(1);
	}
	
	printf("attr.mq_flags:%ld, attr.mq_maxmsg:%ld, attr.mq_msgsize:%ld, attr.mq_curmsgs:%ld \n", attr.mq_flags, attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);
	
	int i = 0;
	while(i < 10)
	{
		msg.len = -1;
		memset(msg.buf, 0, MAXSIZE);
		msg.x = ' ';
		msg.y = -1;
		
		if(mq_receive(id, (char*)&msg, recv_size, &sender ) < 0)
		{
			perror("mq_receive");
			exit(1);
		}
		printf("msg.len:%d, msg.buf:%s, msg.x:%d, msg.y:%d \n", msg.len, msg.buf, msg.x, msg.y);
		i++;
		sleep(2);
	}
	if(mq_close(id) < 0)
	{
		perror("mq_close");
		exit(1);
	}
	
	return 0;
}

