#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <mqueue.h>
#include <errno.h>
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
	

	while(1)
	{
		printf("press enter to continue \n");
		getchar();
		
		printf("please input a string:\n");
		char *p;
		int n;

		errno = 0;
		n = scanf("%m[a-z]", &p);
		if (n == 1)
		{
			printf("read: %s\n", p);
			msg.type = 0;
			memset(msg.buf, 0, MAXSIZE);
			strcpy(&msg.buf, p);
			free(p);
		}
		else if (errno != 0)
		{
			perror("scanf");
			exit(1);
		}
		else
		{
			fprintf(stderr, "No matching characters\n");
			exit(1);
		}
		

		
		if(mq_send(id, (char*)&msg, sizeof(struct MsgType), prio ) < 0)
		{
			perror("mq_send");
			exit(1);
		}

	}
	if(mq_close(id) < 0)
	{
		perror("mq_close");
		exit(1);
	}
	
	mq_unlink(file);
	return 0;
}

