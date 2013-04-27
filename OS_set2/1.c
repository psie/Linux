// a simple chat program for two through a named unix socket
// - fully synchronous (every action is blocking)

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_NAME "/tmp/named_sock"
#define BUFSIZE 256

int xchange(int confd)
{
	char buf[BUFSIZE];
	int len;

	puts("Message: ");
	scanf("%255s", buf);

	if(strcmp(buf, "/quit") == 0)
		return 1;

	write(confd, buf, strlen(buf));

	while((len = recv(confd, buf, BUFSIZE-1, MSG_DONTWAIT)) > 0)
	{
		buf[len] = 0;
		puts("Response: ");
		printf("%s\n", buf);
	}
		
	return 0;
}

int main(int argc, char *argv[])
{
	int sockfd, confd, len;
	struct sockaddr_un addr = { AF_UNIX, SOCK_NAME }; 

	char buf[BUFSIZE];

	if(argc < 2)
	{
		puts("Please specify a working mode (1 - server, 2 - client)");
		return 0;
		}

	sockfd = socket(PF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if(sockfd == -1)
		return 1;

	if(atoi(argv[1]) == 1) // server mode 
	{
		if(bind(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1)
			return 2;
		if(listen(sockfd, 1) == -1)
			return 3;
		puts("Awaiting connections...");	
		while(1)
		{
			len = sizeof(struct sockaddr_un);
			confd = accept(sockfd, (struct sockaddr *) &addr, &len);
			if(confd > -1)
			{	
				puts("Got connection!");
				while(1)
					if(xchange(confd))
					{	
						puts("Exiting.");
						write(confd, "/quit", 5);
						close(confd);
						close(sockfd);
						unlink(SOCK_NAME);
						exit(0);
					}
			}
		}
	}

	else // client mode
	{
        if(connect(sockfd, (struct sockaddr *) &addr, sizeof (struct sockaddr_un)) == -1)
            return 3;	
		puts("Connected.");

		while(1)
			if(xchange(sockfd))
			{	
				puts("Exiting.");
				write(sockfd, "/quit", 5);
				close(sockfd);
				exit(0);
			}
	}
}

