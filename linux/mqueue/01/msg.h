#ifndef __MSG_H__
#define __MSG_H__

#define MAXSIZE 10

struct MsgType 
{
	int len;
	char buf[MAXSIZE];
	char x;
	short y;
};
#endif //__MSG_H__
