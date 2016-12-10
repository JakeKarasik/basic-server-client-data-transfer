#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>

#define PORT_NUM 20000
#define READ_SIZE 256

int main(int argc, char * argv[]){
	
	int socket_fd, accept_fd, bind_response, listen_response, client_addr_len, bytes_received, bytes_sent;
	struct sockaddr_in server_addr, client_addr;
	char buff[READ_SIZE];
	char response[READ_SIZE];

	socket_fd = socket(AF_INET, SOCK_STREAM, 0); //IPv4 connection, initial socket file descriptor

	if (socket_fd < 0) {
		perror("Error opening socket");
		exit(EXIT_FAILURE);
	}

	memset(&server_addr, 0, sizeof(server_addr)); //Make sure server_addr is zero'd

	server_addr.sin_family = AF_INET; //IP_V4 connection
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //get its own ip for binding
	server_addr.sin_port = htons(PORT_NUM); //port to listen on

	bind_response = bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)); //Attempt to bind to port
	
	if (bind_response < 0) {
		perror("Error binding");
		exit(EXIT_FAILURE);
	}

	listen_response = listen(socket_fd, 10); //Max connections = 10 (more of a suggestion then setting...)

	if (listen_response < 0) {
		perror("Error listening");
		exit(EXIT_FAILURE);
	}

	memset(&client_addr, 0, sizeof(client_addr)); //Zero out client_addr
	client_addr_len = sizeof(client_addr);

	char server_name[100];
	server_name[99] = '\0';
	gethostname(server_name, 99);

	printf("Server running on %s:%d\n",server_name,PORT_NUM);
	//printf("Hit enter to close server.\n");
	
	while (1) {
		accept_fd = accept(socket_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);

		if (accept_fd < 0) {
			perror("Error on accept");
			exit(EXIT_FAILURE);
		}

		printf("Accepting connection...\n");

		memset(buff,0,READ_SIZE); //Zero out buff
		memset(response,0,READ_SIZE);

		bytes_received = read(accept_fd,buff,READ_SIZE-1); //receive data from client
		
		if (bytes_received < 0) {
			perror("Error reading from socket");
			close(accept_fd);
			exit(EXIT_FAILURE);
		}

		printf("bytes_received=%d,content=%s\n",bytes_received, buff);
		
		//RECEIVE FORMAT
		//netopen(): "0,FLAG,FILENAME"
		//netclose(): "1, FILE_DESCRIPTOR"
		//netread(): "2, NUM_BYTES_TO_READ, FILE_DESCRIPTOR,"
		//netwrite(): "3, NUM_BYTES_TO_WRITE, BYTES_TO_WRITE"

		//SEND FORMAT
		//netopen(): "S,FILE_DESCRIPTOR" or "F" //success or fail
		//netclose(): 
		//netread(): 
		//netwrite(): 
		switch (buff[0]) {
			case '0': //open
				;
				printf("filename=|%s|,flag=|%c|\n",&buff[4],buff[2]);
				int fd = open(&buff[4],buff[2]-'0');
				printf("fd=%d\n",fd);
				if (fd < 0) {
					//failed to open
					strcat(response,"F");
				} else {
					//successfully opened
					snprintf(response, 32, "S,%d", fd); //file descriptor not bigger than 32 digits
				}
				bytes_sent = write(accept_fd,response,strlen(response)+1);
				break;
			case '1': //close
				
				break;
			case '2': //read

				break;
			case '3': //write

				break;
			default:
				//invalid request
			break;
		}

		if (bytes_sent < 0) {
			perror("Error writing to socket");
			close(accept_fd);
			exit(EXIT_FAILURE);
		}

		close(accept_fd); //close accept file descriptor when finished with incoming connection

	}
	exit(EXIT_SUCCESS);
	return 0;
}