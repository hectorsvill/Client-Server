

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PORT 7890

void error(char *msg)
{
	perror(msg);
	exit(1);
}
// Dumps raw memory in hex byte and printable split format
void dump(const unsigned char *data_buffer, const unsigned int length)
{
	unsigned char byte;
	unsigned int i, j;
	for(i=0; i < length; i++)
	{
		byte = data_buffer[i];
		printf("%02x ", data_buffer[i]); // Display byte in hex. if(((i%16)==15) || (i==length-1)) {
		for (j=0; j < 15-(i%16); j++)
			printf(" ");
		printf("| ");
		// Display printable bytes from line.
		for(j=(i-(i%16)); j <= i; j++)
		{
			byte = data_buffer[j];
			if((byte > 31) && (byte < 127)) // Outside printable char range
				printf("%c", byte);
			else
				printf(".");
		}
		printf("\n"); // End of the dump line (each line is 16 bytes) } // End if
		} // End for
}

int main(void)
{
	int sockfd, new_sockfd;
	struct sockaddr_in host_addr, client_addr;
	socklen_t sin_size;
	int recv_length = 1, yes = 1;
	char buffer[1024];

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		error("in socket");
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == 1)
		error("setting socket option SO_REUSEADDR");
	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(PORT);
	host_addr.sin_addr.s_addr = 0;
	memset(&(host_addr.sin_zero), '\0', 8);
	if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1)
		error("error binding");
	if (listen(sockfd, 5) == -1)
		error("listening on socket");

		//accept loop
	while(1)
	{
		sin_size = sizeof(struct sockaddr_in);
		new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
		if(new_sockfd == -1)
			error("accepting connection");
		printf("server: got connection from %s port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		send(new_sockfd, "Hello World!\n", 13, 0);
		recv_length = recv(new_sockfd, &buffer, 1024, 0);
		while (recv_length > 0)
		{
				printf("RECV:%d bytes\n", recv_length);
				dump(buffer, recv_length);
				recv_length = recv(new_sockfd, &buffer, 1024, 0);
		}
		close(new_sockfd);
	}
	return (0);
}
