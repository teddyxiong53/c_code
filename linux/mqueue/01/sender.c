#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <mqueue.h>
#include "msg.h"


int main()
{
	mqd_t id;
	struct MsgType msg;
	struct mq_attr attr;
	const char *file = "/posix";
	int ret = 0;
	unsigned int prio = 1;
	id = mq_open(file, O_RDWR|O_CREAT, 0666, NULL);
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
		msg.len = i;
		memset(msg.buf, 0, MAXSIZE);
		sprintf(msg.buf, "0x%x", i);
		msg.x = (char)(i+'a');
		msg.y = (short)(i+100);
		
		if(mq_send(id, (char*)&msg, sizeof(struct MsgType), prio ) < 0)
		{
			perror("mq_send");
			exit(1);
		}
		i++;
		sleep(1);
	}
	sleep(30);
	if(mq_close(id) < 0)
	{
		perror("mq_close");
		exit(1);
	}
	
	mq_unlink(file);
	return 0;
}

