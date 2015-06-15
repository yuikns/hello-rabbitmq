#ifndef __ARGCV_NW_H__
#define __ARGCV_NW_H__

#include <sys/socket.h> // for socklen_t


#define ARGCVNW_IN_DEBUG

#ifndef LISTENQ
#define LISTENQ 1024 // 
#endif // LISTENQ

typedef struct sockaddr SA;

// Make this header file easier to include in C++ code
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *
 * TCP 
 *
 ******************************************************************************/

/**
 * const char * host : host name  or IP
 * const char * serv : port or service 
 */
int tcp_connect(const char * host , const char * serv);


/**
 * const char * hostname : host name  or IP
 * const char * service : port or service
 * socklen_t * addrlenp : size of protocol address
 */
int tcp_listen(const char * hostname , const char * service , socklen_t * addrlenp);



/*******************************************************************************
 *
 * UDP 
 *
 ******************************************************************************/

int udp_client(const char * host, const char * serv,SA ** saptr,socklen_t * addrlenp); 

int udp_connect(const char * host, const char * serv);

int udp_server(const char * host,const char * serv,socklen_t *addrlenp);




/*******************************************************************************
 *
 * SCTP 
 *
 ******************************************************************************/




/*******************************************************************************
 *
 * SYSTEM 
 *
 ******************************************************************************/
// defined in P. 105 , unix network programming
typedef void sigfunc(int);
sigfunc * _signal(int signo,sigfunc * func);
void sig_chld(int signo);


/*******************************************************************************
 *
 * DAEMON 
 *
 ******************************************************************************/
#ifndef MAXFD
#define MAXFD 64
#endif // MAXFD

#ifndef PROJ_WORK_DIR
#define PROJ_WORK_DIR "/"
#endif //PROJ_WORK_DIR

extern int daemon_proc;  // defined in error.c ?

int daemon_init(const char * ident);

void daemon_stop();





/*******************************************************************************
 *
 * TOOLS IN UNPV13E
 *
 ******************************************************************************/
char * sock_ntop_host(const struct sockaddr *sa, socklen_t salen); // check : HAVE_HEADER_NET_IF_DL_H


#ifdef __cplusplus
}
#endif


#endif // __ARGCV_NW_H__
