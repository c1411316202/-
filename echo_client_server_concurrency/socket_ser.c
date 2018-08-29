#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#define ERR_EXIT(m) \
	do \
	{ \
		perror(m) ;\
		exit(EXIT_FAILURE) ; \
	} while(0) 
void handler(int sig)
{
	printf("recv a sig = %d" , sig) ;
	exit(EXIT_SUCCESS) ;
}
int main(void)
{
	int listenfd ;
	if( (listenfd = socket( PF_INET , SOCK_STREAM , IPPROTO_TCP )) < 0 )
		ERR_EXIT("socket");
	struct sockaddr_in servaddr ;
	memset( &servaddr ,0,sizeof(servaddr) ) ;
	servaddr.sin_family = PF_INET ;
	servaddr.sin_port = htons(6666) ;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1") ;

	int on = 1 ;
	if( setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) < 0 )
	  ERR_EXIT("setsockopt") ;
	
	if( (bind( listenfd , ( struct sockaddr*) &servaddr , sizeof(servaddr) ) ) < 0 ) 
		ERR_EXIT("bind");
	if(listen( listenfd , SOMAXCONN ) < 0 ) 
		ERR_EXIT("listen");
	struct sockaddr_in peeraddr ;
	socklen_t peerlen = sizeof(peeraddr) ;
	int conn ;

	pid_t pid ;
	

	if( ( conn = accept( listenfd , ( struct sockaddr* )&peeraddr , &peerlen ) ) < 0 )
		ERR_EXIT("accept") ;

	printf( "ip = %s port = %d \n",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port) ) ;
	    
	pid = fork() ;
	if( pid == -1 )
	    ERR_EXIT("fork") ;
	char recvbuf[1024] ;
	char sendbuf[1024] ;
	if( pid == 0 )
	{
		while(1)
		  {
		    memset( recvbuf , 0 , sizeof(recvbuf) ) ;
		    int ret = read(conn , recvbuf , sizeof(recvbuf) ) ;
		    if( ret == 0 )
		      {
			printf("client_close\n") ;
			break ;
		      }
		    if( ret == -1 )
		      ERR_EXIT("read") ;
		    fputs( recvbuf , stdout) ;
		  }
		kill(pid,SIGUSR1) ;
		close(conn) ;
		close(listenfd) ;
		exit(EXIT_SUCCESS) ;
	}
	else 
	{
		signal(SIGUSR1,handler) ;
		while( fgets( sendbuf , sizeof(sendbuf) , stdin ) != NULL )
		{
			write( conn , sendbuf , sizeof(recvbuf) ) ;
			memset( sendbuf , 0 , sizeof(sendbuf) ) ;
		};
	}
	
	close(conn) ;
	close(listenfd) ;
	return 0 ;
}
