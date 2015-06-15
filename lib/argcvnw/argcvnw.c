#include "argcvnw.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h> // setsid
#include <string.h>
#include <stdio.h>
#include <stdlib.h> // malloc

#include <syslog.h> // syslog
#include <signal.h> // signal
#include <sys/stat.h> // open O_RDONL
#include <fcntl.h> // open

#include <sys/wait.h> // wait

#include <netdb.h>
#include <arpa/inet.h>


/*******************************************************************************
 *
 * TCP PART
 *
 ******************************************************************************/

/**
 * const char * host : host name  or IP
 * const char * serv : port or service 
 */
int tcp_connect(const char * host , const char * serv)
{
	int sockfd = 0;
	int n;
	
	struct addrinfo hints;
	struct addrinfo *res;
	struct addrinfo *ressave;
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if( (n = getaddrinfo(host,serv,&hints,&res)) != 0 )
	{
		if(daemon_proc)
		{
			syslog(LOG_ERR,"tcp_connect error for %s , %s:%s \n",
				gai_strerror(n),host,serv);
		}else
		{
			fprintf(stderr,"tcp_connect error for %s , %s:%s \n",
				gai_strerror(n),host,serv);
		}
		return 0;
	}

	ressave = res;

	do {
		sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);

		// error , try next one
		if(sockfd < 0) continue;

		// connect , if return zero ,
		// it will be judged as success and break the loop
		if(connect(sockfd,res->ai_addr,res->ai_addrlen) == 0 ) break;

		// connect error ,
		// close this one and try next
		close(sockfd);

	}while((res = res->ai_next) != NULL);

	if(res == NULL) // errno from final socket() or connect()
	{
		if(daemon_proc)
		{
			syslog(LOG_ERR,"tcp_connect error for %s:%s \n",host,serv);
		}else
		{
			fprintf(stderr,"tcp_connect error for %s:%s \n",host,serv);
		}
		return 0;
	}
	freeaddrinfo(ressave);
	return sockfd;
}


/**
 * const char * host : host name  or IP
 * const char * serv : port or service
 * socklen_t * addrlenp : size of protocol address
 */
int tcp_listen(const char * host , const char * serv , socklen_t * addrlenp)
{
	int listenfd = 0;
	int n;
	const int on = 1;
	struct addrinfo hints;
	struct addrinfo * res;
	struct addrinfo * ressave;
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if((n = getaddrinfo(host,serv,&hints,&res)) != 0)
	{
		if(daemon_proc)
		{
			syslog(LOG_ERR,"tcp_listen error for %s , %s:%s \n",
				gai_strerror(n),host,serv);
		}else
		{
			fprintf(stderr,"tcp_listen error for %s , %s:%s \n",
				gai_strerror(n),host,serv);
		}
		return 0;
	}
	ressave = res;
	do {
		// 
		listenfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		// error , try next one
		if(listenfd < 0) continue;
		// 
		setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
		// try bind , if equals zero , 
		// it will be judged as success and break the loop
		if(bind(listenfd,res->ai_addr,res->ai_addrlen) == 0) break;
		// bind error , close & try next one
		close(listenfd);
	}while( (res = res->ai_next) != NULL);
	if(res == NULL) // errno from final socket() or bind()
	{
		if(daemon_proc)
		{
			syslog(LOG_ERR,"tcp_listen error for %s:%s \n",host,serv);
		}else
		{
			fprintf(stderr,"tcp_listen error for %s:%s \n",host,serv);
		}
		return 0;
	}
	// listen LISTENQ is customed
	listen(listenfd,LISTENQ);
	if(addrlenp) *addrlenp = res->ai_addrlen;
	freeaddrinfo(ressave);
	return listenfd;
}


/*******************************************************************************
 *
 * UDP PART
 *
 ******************************************************************************/

int udp_client(const char * host, const char * serv,SA ** saptr,socklen_t * addrlenp)
{
	int sockfd = 0;
	int n;
	struct addrinfo hints;
	struct addrinfo *res;
	struct addrinfo *ressave;
	
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	if( (n = getaddrinfo(host,serv,&hints,&res)) != 0)
	{
		if(daemon_proc)
		{
			syslog(LOG_ERR,"udp_client error for %s , %s:%s [%s,%d]\n",
				gai_strerror(n),host,serv,__FILE__,__LINE__);
		}else
		{
			fprintf(stderr,"udp_client error for %s , %s:%s [%s,%d]\n",
				gai_strerror(n),host,serv,__FILE__,__LINE__);
		}
		
		return 0;
	}
	ressave = res;
	
	do{
		sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if(sockfd >= 0) break;	
	}while ((res = res-> ai_next) != NULL);
	
	if(res == NULL) // errno from final socket() or connect()
	{
		fprintf(stderr,"udp_client error for %s:%s [%s,%d]\n",host,serv,__FILE__,__LINE__);
		if(daemon_proc)
		{
			syslog(LOG_ERR,"udp_client error for %s:%s [%s,%d]\n",host,serv,__FILE__,__LINE__);
		}else
		{
			fprintf(stderr,"udp_client error for %s:%s [%s,%d]\n",host,serv,__FILE__,__LINE__);
		}
		return 0;
	}
	
	* saptr = (SA *)malloc(res->ai_addrlen);
	memcpy(*saptr,res->ai_addr,res->ai_addrlen);
	* addrlenp = res->ai_addrlen;
	
	freeaddrinfo(ressave);
	
	return sockfd;
}


int udp_connect(const char * host, const char * serv)
{
	int sockfd = 0;
	int n;
	struct addrinfo hints;
	struct addrinfo * res;
	struct addrinfo * ressave;
	
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	if( (n = getaddrinfo(host,serv,&hints,&res)) != 0)
	{
		if(daemon_proc)
		{
			syslog(LOG_ERR,"udp_connect error for %s , %s:%s [%s,%d]\n",
				gai_strerror(n),host,serv,__FILE__,__LINE__);
		}else
		{
			fprintf(stderr,"udp_connect error for %s , %s:%s [%s,%d]\n",
				gai_strerror(n),host,serv,__FILE__,__LINE__);
		}
		return 0;
	}
	ressave = res;
	
	do{
		sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if(sockfd < 0 ) continue;
		if(connect(sockfd,res->ai_addr,res->ai_addrlen) == 0)
			break;
		close(sockfd);
	}while ((res = res-> ai_next) != NULL);
	
	if(res == NULL) // errno from final socket() or connect()
	{
		if(daemon_proc)
		{
			syslog(LOG_ERR,"udp_connect error for %s:%s [%s,%d]\n",host,serv,__FILE__,__LINE__);
		}else
		{
			fprintf(stderr,"udp_connect error for %s:%s [%s,%d]\n",host,serv,__FILE__,__LINE__);
		}
		return 0;
	}

	freeaddrinfo(ressave);
	
	return sockfd;

}

int udp_server(const char * host,const char * serv,socklen_t *addrlenp)
{
	int sockfd = 0;
	int n;
	struct addrinfo hints;
	struct addrinfo *res;
	struct addrinfo *ressave;
	
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	if((n = getaddrinfo(host,serv,&hints,&res)) != 0)
	{
		if(daemon_proc)
		{
			syslog(LOG_ERR,"udp_server error for %s , %s:%s [%s,%d]\n",
				gai_strerror(n),host,serv,__FILE__,__LINE__);
		}else
		{
			fprintf(stderr,"udp_server error for %s , %s:%s [%s,%d]\n",
				gai_strerror(n),host,serv,__FILE__,__LINE__);
		}
		return 0;
	}
	
	ressave = res;
	
	do{
		sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if (sockfd < 0 ) continue;
		if (bind(sockfd,res->ai_addr,res->ai_addrlen) == 0) break;
		close(sockfd);
	}while ( (res = res->ai_next) != NULL);
	
	if(res == NULL)
	{
		if(daemon_proc)
		{
			syslog(LOG_ERR,"udp_server error for %s:%s [%s,%d]\n",
				host,serv,__FILE__,__LINE__);
		}else
		{
			fprintf(stderr,"udp_server error for %s:%s [%s,%d]\n",
				host,serv,__FILE__,__LINE__);
		}
		return 0;
	}
	
	if(addrlenp) *addrlenp = res->ai_addrlen;
	freeaddrinfo(ressave);
	return sockfd;
}




/*******************************************************************************
 *
 * SCTP PART
 *
 ******************************************************************************/




/*******************************************************************************
 *
 * SYSTEM PART
 *
 ******************************************************************************/
sigfunc * _signal(int signo,sigfunc * func)
{
	struct sigaction act; // signal.h
	struct sigaction oact;
	printf("signal called  ...\n");
	
	act.sa_handler = func;
	sigemptyset(&act.sa_mask); // signal.h
	act.sa_flags = 0;
	
	if(signo == SIGALRM) // 
	{
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT; // SunOS 4.x
#endif //SA_INTERRUPT
	}else
	{
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART; // SVR4, 4.4BSD
#endif //SA_RESTART
	}
	if(sigaction(signo,&act,&oact) < 0 )
		return SIG_ERR;
	return oact.sa_handler;
}

void sig_chld(int signo)
{
	pid_t pid;
	int stat;
	while( (pid = waitpid(-1,&stat,WNOHANG)) > 0 )
	{
#ifdef ARGCVNW_IN_DEBUG
		if(daemon_proc)
		{
			syslog(LOG_DEBUG,"child %d terminated \n",pid);
		}else
		{
			fprintf(stdout,"child %d terminated \n",pid);
		}
#endif // ARGCVNW_IN_DEBUG
	}
	return;
}




/*******************************************************************************
 *
 * DAEMON PART
 *
 ******************************************************************************/
int daemon_proc = 0; // for err_XXX() functions

int daemon_init(const char * ident)
{
	openlog(ident,LOG_PID,LOG_USER);
	//openlog(ident,LOG_CONS|LOG_PID,LOG_USER);
	syslog(LOG_INFO,"daemon starting");
	int i;
	pid_t pid;

	if((pid = fork()) < 0 )
	{
		return -1;
	}
	else if(pid)
	{
		_exit(0); // parent terminates
	}

	// child 1 continues ... 
	//
	if(setsid() < 0) // become session leader
	{
		return -1;		
	}

	signal(SIGHUP,SIG_IGN);

	if((pid = fork()) < 0)
	{
		return -1;
	}
	else if(pid)
	{
		_exit(0);
	}
	// child 2 continues ...
	daemon_proc = 1; // for err_XXX() functions

	chdir(PROJ_WORK_DIR);

	// close off file descriptors
	for(i = 0 ; i < MAXFD ; i ++)
	{
		close(i);
	}

	open("/dev/null",O_RDONLY);
	open("/dev/null",O_RDWR);
	open("/dev/null",O_RDWR);

	syslog(LOG_INFO,"daemon started");
	return 0;
}

void daemon_stop()
{
	syslog(LOG_INFO,"daemon stopped");
	closelog();
}














