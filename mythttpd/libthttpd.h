#ifndef __LIBTHTTPD_H__
#define __LIBTHTTPD_H__

#include <sys/socket.h>
#include <netinet/in.h>

typedef union
{
	struct sockaddr sa;
	struct sockaddr_in sa_in;
} httpd_sockaddr;


#endif