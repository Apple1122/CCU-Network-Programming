#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>

#define SERV_PORT 8081
#define STDIN 0 // standard input 的 file descriptor

int
main(int argc, char **argv)
{
	int					i, maxi, maxfd, listenfd, connfd, sockfd;
	int					nready, client[10];
	ssize_t				n;
	fd_set				writefds;
	char				buf[1024];
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
    
    // socket connect
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
    
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	bind(listenfd, (struct sockaddr_in *) &servaddr, sizeof(servaddr));

	listen(listenfd, 5);
        
    struct timeval tv;
    
    tv.tv_sec = 10;
    tv.tv_usec = 500000;
    
    FD_ZERO(&writefds);
    FD_SET(STDIN, &writefds);
    
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world! Body";
    
    for( ; ; )
    {
        select(STDIN+1, NULL, &writefds, NULL, &tv);
        connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);
        
        char buffer[30000] = {0};
        read( connfd , buffer, 30000);
        printf("%s\n",buffer );
        write(connfd , hello , strlen(hello));
        printf("------------------Hello message sent-------------------");
        close(connfd);
        
    }
    
    
    
    
    
//    // 不用管 writefds 與 exceptfds：
//    select(STDIN+1, &readfds, NULL, NULL, &tv);
//
//    if (FD_ISSET(STDIN, &readfds))
//        printf("A key was pressed!\n");
//    else
//        printf("Timed out.\n");
//    return 0;
    
    
    
    
}
/* end fig02 */
