#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h> 
#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>
#include    <unistd.h>
#include    <sys/wait.h>
#define BUFFER_SIZE 300000
#define PORT 8080


int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	char buffer[BUFFER_SIZE];
    
    
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd == 0)
		printf("Fail to connect a socket");
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //IP address
	servaddr.sin_port        = htons(8080);



	bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	//listen(listenfd, LISTENQ);
    
	listen(listenfd, 10);

	char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world! Body";
    
	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);
        
        printf("server waiting\n");
        if ( (childpid = fork()) == 0) {    /* child process */
            close(listenfd);    /* close listening socket */
//            str_echo(connfd);    /* process the request */
            
             read(connfd, buffer, BUFFER_SIZE);
             printf("%s\n", buffer);
             write(connfd, hello, strlen(hello));
			 
			 
//             // buf = server open file
//              FILE *file = fopen("./index.html", "r");
//              int reset = fread(buffer, 52 + 1, 1, file);
//
//              if(file)
//                printf("file exist");
//
//                write(connfd, buffer, 52);
//              //fwrite(file, sizeof(buffer), 1, &listenfd);
//             //  write(buf)  to client
            
            
            if ((childpid = fork()) < 0);
            else if (childpid > 0)
                exit(0); //first child die
            sleep(2);
            printf("second child, parent pid=%ld\n", (long)getppid());

			exit(0);
        }
		close(connfd);			/* parent closes connected socket */
	}
}
