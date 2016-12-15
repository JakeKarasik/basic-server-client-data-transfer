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
#include <pthread.h>

#define PORT_NUM 15151
#define READ_SIZE 4096 //max write size

typedef struct data_ {
	int client_fd;
	char operation;
	char * filename;
	int flag;
	int file_desc;
	int num_bytes;
	char * bytes;
} data;

void * thread_worker(data * request_data) {
	int errno_max_digits = 3, bytes_sent;
	char response[READ_SIZE/2];

	memset(response,0,READ_SIZE/2);
	response[(READ_SIZE/2)-1] = '\0';

	//Get ready to error check
		errno = 0;
		h_errno = 0;
		
		char errno_buff[errno_max_digits+1];
		char h_errno_buff[errno_max_digits+1];
		memset(errno_buff,0,errno_max_digits+1);
		memset(h_errno_buff,0,errno_max_digits+1);
		errno_buff[errno_max_digits] = '\0';
		h_errno_buff[errno_max_digits] = '\0';

	//Switch based on request type: open=0, close=1, read=2, write=3
		switch (request_data->operation) {
			case '0': //open
				;
				int fd = open(request_data->filename,request_data->flag);
				free(request_data->filename);
				if (fd < 0) {
					//failed to open
					strcat(response,"F,");
					snprintf(errno_buff, 4, "%d", errno); //max errno digits = 3 + null term
					snprintf(h_errno_buff, 4, "%d", h_errno); //max errno digits = 3 + null term
					strcat(response, errno_buff);
					strcat(response, ",");
					strcat(response, h_errno_buff);
				} else {
					//successfully opened
					snprintf(response, snprintf(NULL, 0, "%d", fd) + 2 + 1, "S,%d", fd); //file descriptor + 'S,' + null term
				}
				bytes_sent = write(request_data->client_fd,response,strlen(response)+1);
				break;
			case '1': //close
				;
				int close_resp = close(request_data->file_desc);

				if (close_resp < 0) {
					//failed to close
					strcat(response,"F,");
					snprintf(errno_buff, 4, "%d", errno); //max errno digits = 3 + null term
					snprintf(h_errno_buff, 4, "%d", h_errno); //max errno digits = 3 + null term
					strcat(response, errno_buff);
					strcat(response, ",");
					strcat(response, h_errno_buff);
				} else {
					//successfully closed
					strcat(response,"S");
				}
				bytes_sent = write(request_data->client_fd,response,strlen(response)+1);
				break;
			case '2': //read
				;

				int bytes_to_read_length = snprintf(NULL, 0, "%d", request_data->num_bytes);
				int read_size = request_data->num_bytes;
				void * read_buffer = malloc(read_size);//bytes
				char * success_buff = malloc(read_size +2+bytes_to_read_length+1 +1);
				memset(read_buffer,0,read_size);
				memset(success_buff,0,read_size + 2 + bytes_to_read_length + 1 + 1);

				ssize_t read_resp = read(request_data->file_desc,read_buffer,request_data->num_bytes);
				//printf("read_resp=%d\n",(int)read_resp);
				if (read_resp < 0) {
					//failed to read
					strcat(response,"F,");
					snprintf(errno_buff, 4, "%d", errno); //max errno digits = 3 + null term
					snprintf(h_errno_buff, 4, "%d", h_errno); //max errno digits = 3 + null term
					strcat(response, errno_buff);
					strcat(response, ",");
					strcat(response, h_errno_buff);
					bytes_sent = write(request_data->client_fd,response,strlen(response)+1);
				} else {
					//successfully read
					strcat(success_buff,"S,");
					char bytes_to_read_buff[bytes_to_read_length];
					snprintf(bytes_to_read_buff, bytes_to_read_length+1, "%d", (int)read_resp);
					strcat(success_buff,bytes_to_read_buff);
					strcat(success_buff,",");
					//strcat(success_buff,read_buffer);
					memcpy(&success_buff[2 + bytes_to_read_length + 1 + 1],read_buffer,(int)read_resp);
					
					//printf("int serv=%d\n",*(int *)((read_buffer)));
					//printf("success_buff=%d\n",*(int *)(success_buff+1 + bytes_to_read_length + 2 + 1)); // "S" + bytes_to_read_len + 2 commas + nullterm
					bytes_sent = write(request_data->client_fd,success_buff,read_resp+1+2+bytes_to_read_length+1); //bytes + S + 2 commas + bytes_to_read_len + nullterm
				}
				free(read_buffer);
				free(success_buff);
				break;
			case '3': //write
				;

				ssize_t write_resp = write(request_data->file_desc,request_data->bytes,request_data->num_bytes);

				if (write_resp < 0) {
					//failed to write
					strcat(response,"F,");
					snprintf(errno_buff, 4, "%d", errno); //max errno digits = 3 + null term
					snprintf(h_errno_buff, 4, "%d", h_errno); //max errno digits = 3 + null term
					strcat(response, errno_buff);
					strcat(response, ",");
					strcat(response, h_errno_buff);
				} else {
					//successfully wrote
					strcat(response, "S,");
					int bytes_to_write_length = snprintf(NULL, 0, "%d", (int)write_resp);
					char bytes_to_write_buff[bytes_to_write_length];
					snprintf(bytes_to_write_buff, bytes_to_write_length+1, "%d", (int)write_resp);
					strcat(response,bytes_to_write_buff);
				}

				bytes_sent = write(request_data->client_fd,response,strlen(response)+1);
				free(request_data->bytes);
				break;
			default:
				//invalid request
			break;
		}

		if (bytes_sent < 0) {
			perror("Error writing to socket");
			close(request_data->client_fd);
			free(request_data);
			exit(EXIT_FAILURE);
		}

		close(request_data->client_fd); //close accept file descriptor when finished with incoming connection
		free(request_data); //free passed in data
		return NULL;
}

int main(int argc, char * argv[]){
	
	int socket_fd, accept_fd, bind_response, listen_response, client_addr_len, bytes_received;
	struct sockaddr_in server_addr, client_addr;
	char buff[READ_SIZE];
	char * part;

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
	
	while (1) {
		accept_fd = accept(socket_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);

		if (accept_fd < 0) {
			perror("Error on accept");
			pthread_exit(NULL);
		}

		memset(buff,0,READ_SIZE); //Zero out buff and response

		bytes_received = read(accept_fd,buff,READ_SIZE); //receive data from client
		
		if (bytes_received < 0) {
			perror("Error reading from socket");
			close(accept_fd);
			pthread_exit(NULL);
		}

		//printf("bytes_received=%d,content=%s\n",bytes_received, buff);
		
		//RECEIVE FORMAT
		//netopen(): "0,FLAG,FILENAME"
		//netclose(): "1, FILE_DESCRIPTOR"
		//netread(): "2, FILE_DESCRIPTOR, NUM_BYTES_TO_READ"
		//netwrite(): "3, FILE_DESCRIPTOR, NUM_BYTES_TO_WRITE,"BYTES_TO_WRITE

		//SEND FORMAT
		//netopen(): "S,FILE_DESCRIPTOR" or "F,ERRNO,H_ERRNO" //success or fail
		//netclose(): "S" or "F,ERRNO,H_ERRNO" //success or fail
		//netread():  "S,NUM_BYTES_READ,"CONTENT or "F,ERRNO,H_ERRNO" //success or fail
		//netwrite(): "S,NUM_BYTES_WRITTEN" or "F,ERRNO,H_ERRNO" //success or fail

		
		data * thread_data = malloc(sizeof(data));
		thread_data->operation = buff[0];
		thread_data->client_fd = accept_fd;
	/*
		char operation;
		char * filename;
		int flag;
		int file_desc;
		int bytes_to_read;
		int bytes_to_write;
		int mode;
	*/
		switch (thread_data->operation) {
			case '0': //open
				thread_data->filename = malloc(strlen(&buff[4])+1);
				strcpy(thread_data->filename, &buff[4]);
				thread_data->flag = atoi(&buff[2]);
				break;
			case '1': //close
				thread_data->file_desc = atoi(&buff[2]);
				break;
			case '2': //read
				part = strtok(buff, ",");
	    		
	    		part = strtok(NULL, ",");
	    		thread_data->file_desc = atoi(part);
	    		
	    		part = strtok(NULL, ",");
				thread_data->num_bytes = atoi(part);
				break;
			case '3': //write
				part = strtok(buff, ",");
	    		
	    		part = strtok(NULL, ",");
	    		thread_data->file_desc = atoi(part);
	    		
	    		part = strtok(NULL, ",");
	    		thread_data->bytes = malloc(atoi(part));
				thread_data->num_bytes = atoi(part);

				memset(thread_data->bytes,0,thread_data->num_bytes);
				memcpy(thread_data->bytes,&buff[bytes_received-thread_data->num_bytes],thread_data->num_bytes);
				//printf("int=%d\n",*(int *)((thread_data->bytes)));
				break;
			default:
				//invalid request
				break;
		}

		pthread_t thread_id;
		pthread_create(&thread_id, NULL, (void * (*)(void *))thread_worker, thread_data); //Create new thread

		pthread_detach(thread_id);	

	}
	pthread_exit(NULL);
	return 0;
}