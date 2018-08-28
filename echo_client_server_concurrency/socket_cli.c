#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define ERR_EXIT(m) \
	do \
	{ \
		perror(m) ;\
		exit(EXIT_FAILURE) ; \
	} while(0) 

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
	while( fgets( sendbuf , sizeof(sendbuf) , stdin ) != NULL )
	{
		write( sock , sendbuf , strlen(sendbuf) ) ;
		read( sock , recvbuf , sizeof(recvbuf) ) ;
		
		fputs( recvbuf , stdout ) ;
	}
	close(sock) ;
	return 0 ;
}
