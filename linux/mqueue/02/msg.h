#ifndef __MSG_H__
#define __MSG_H__

#define MAXSIZE 1024

struct MsgType 
{
	int type;
	char buf[MAXSIZE];
	char x;
	short y;
};
#endif //__MSG_H__
