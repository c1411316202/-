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
	printf("recv a sig = %d\n",sig) ;
	exit(EXIT_SUCCESS) ;
}

int main(void)
{
	int sock ;
	if( (sock = socket( PF_INET , SOCK_STREAM , 0 ) )  < 0 )
		ERR_EXIT("SOCK_FAIL");
	struct sockaddr_in servaddr ;
	memset( &servaddr ,0,sizeof(servaddr) ) ;
	servaddr.sin_family = AF_INET ;
	servaddr.sin_port = htons(6666) ;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1") ;
	
	if( (connect(sock,(struct sockaddr*)&servaddr , sizeof(servaddr) ) ) < 0  )
		ERR_EXIT("connect") ;
	
	char sendbuf[1024] = {0} ;
	char recvbuf[1024] = {0} ;
	pid_t pid ;
	pid = fork() ;
	if( 0 == pid )
	{
		signal(SIGUSR1,handler) ;
		while( fgets( sendbuf , sizeof(sendbuf) , stdin ) != NULL )
		{
			write( sock , sendbuf , strlen(sendbuf) ) ;
			memset( recvbuf , 0 , sizeof(recvbuf) ) ;
		}
	}
	else
		while(1)
		{
		        memset( recvbuf , 0 , sizeof(recvbuf) ) ;
			int ret = read( sock , recvbuf , sizeof(recvbuf) ) ;
			if( -1 == ret )
			  ERR_EXIT("read") ;
			else if( ret == 0 )
			  {
			    printf("peer close\n") ;
			    kill( pid,SIGUSR1 ) ;
			    break ;
			  }
			fputs( recvbuf , stdout ) ;
		}
	close(sock) ;
	return 0 ;
}
