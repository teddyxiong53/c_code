#include <stdio.h>
#include <syslog.h>
#include <pwd.h>
#include <sys/param.h>


#include "version.h"
#include "libthttpd.h"

extern char *optarg;
extern int optind, opterr, optopt;

static char *argv0;

static int debug;
static unsigned short port;
static char *hostname;
static char *throttlefile;
static char *user;
static char *pidfile;
static int do_chroot;

typedef struct 
{
	char *pattern;
	
}throttletab;

static throttletab *throttles;
static int numthrottles;
static int maxthrottles;

static void usage()
{
    fprintf(stderr, 
        "useage:\n\
            -h host ip\n\
            -p port number\n\
            -D debug mode\n\
            -V version\n\
            -l logfile\n\
            -i pidfile\n\
        \n"
    );
    exit(1);
}
static void parse_args(int argc, char **argv)
{
	char c;
	while((c = getopt(argc, argv, "DVh:p:i:")) != -1)
	{
		switch(c)
		{
			case 'D':
				printf("[xhl]:%s, %d \n", __func__, __LINE__);
				
				break;
			case 'V':
				printf("[xhl]:%s, %d \n", __func__, __LINE__);
				
				break;
			case 'h':
				printf("[xhl]:%s, %d \n", __func__, __LINE__);
				
				break;
			case 'p':
				printf("[xhl]:%s, %d \n", __func__, __LINE__);
				
				break;
            
			case 'l':
				printf("[xhl]:%s, %d \n", __func__, __LINE__);
				
				break;
            
			case 'i':
				printf("[xhl]:%s, %d \n", __func__, __LINE__);
				pidfile = optarg;
                printf("pidfile:%s \n", pidfile);
				break;
			default:
				break;
		}
	}
}

static lookup_hostname(httpd_sockaddr *sa4P, size_t sa4_len, int *gotv4P)
{
	struct hostentry *he;
	memset(sa4P, 0, sa4_len);
	sa4P->sa.sa_family = AF_INET;
	if(hostname == NULL)
	{
		sa4P->sa_in.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		sa4P->sa_in.sin_addr.s_addr = inet_addr(hostname);
		
	}
	sa4P->sa_in.sin_port = htons(port);
	*gotv4P = 1;
	
}

static void handle_term(int sig)
{
    
}
static void handle_chld(int sig)
{
    
}

static void handle_hup(int sig)
{
    
}

static void handle_usr1(int sig)
{
    
}

static void handle_usr2(int sig)
{
    
}

static void handle_alm(int sig)
{
    
}





int main(int argc, char **argv)
{
	char *cp;
	int gotv4 = 0;
	httpd_sockaddr sa4 = {};
    struct passwd *pwd;
    uid_t uid = 32767;
    gid_t gid = 32767;
    char cwd[MAXPATHLEN+1];
    
	argv0 = argv[0];
	cp = strrchr(argv0, '/');//search from the last /
	
	//get app name, without '/'
	if(cp != NULL)
	{
		//found
		cp++;//skip '/'
	}
	else
	{
		cp = argv0;
	}
	//openlog(cp, LOG_NDELAY|LOG_PID, LOG_DAEMON);
	parse_args(argc, argv);
	
	tzset();
	//lookup hostname now ,in case we chroot
	lookup_hostname(&sa4, sizeof(sa4), &gotv4);
	if(!gotv4)
	{
		syslog(LOG_ERR, "can't find valid address");
		exit(1);
	}
	
	numthrottles = 0;
	maxthrottles = 0;
	throttles = (throttletab *)0;
	
	if(getuid() == 0)
	{
		printf("run as root \n");
        pwd = getpwnam(user);
        if(pwd == NULL)
        {
            syslog(LOG_CRIT, "unknown user - '%80s' \n", user);
            exit(1);
        }
        uid = pwd->pw_uid;
        gid = pwd->pw_gid;
        
        
	}
    getcwd(cwd, sizeof(cwd)-1);
    if(cwd[strlen(cwd)-1] != '/')
    {
        strcat(cwd, "/");
    }
    if(!debug)
    {
        printf("not debug mode \n");
    }
    else
    {
        setsid();
    }

    if(pidfile != NULL)
    {
        FILE *pidfp = fopen(pidfile, "w");
        if(pidfp == NULL)
        {
            syslog("LOG_CRIT", "%.80s - %m", pidfile);
            exit(1);
        }
        fprintf(pidfp, "%d\n", getpid());
        fclose(pidfp);
    }
    
    if(do_chroot)
    {
        //TODO
    }
    //signal register
    signal(SIGTERM, handle_term);
    signal(SIGINT, handle_term);
    signal(SIGCHLD, handle_chld);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP, handle_hup);
    signal(SIGUSR1, handle_usr1);
    signal(SIGUSR2, handle_usr2);
    signal(SIGALRM, handle_alm);
    
    closelog();
    printf("start finish \n");
	return 0;
}
