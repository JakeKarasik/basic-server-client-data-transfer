#include "libnetfiles.h"

int socket_fd = -1;

/**
    Used to open a file on the server from the client.

    file: 		The file to open on the server
   	flags: 		O_RDONLY | O_WRONLY | O_RDWR
   
    returns: 	The file descriptor, or -1 if an error occurred
*/
int netopen(const char * file, int flag){
	int buff_size = strlen(file)+1+2+2;//filename + nullterm + 'operation,' + 'flag,'
	char buff[buff_size];
	int receive_data_size = 32;
	char received_data[receive_data_size];
	int file_desc = -1, bytes_sent, bytes_received;
	char status = 'F'; //fail by default

	if (socket_fd == -1) {
		fprintf(stderr,"Error: Connection does not exist.");
		return -1;
	}

	//Build message with format: "OPERATION,FLAG,FILENAME"
	memset(&buff,0,buff_size);
	strcat(buff,"0,"); //0 = open
	char flag_buff[3];
	snprintf(flag_buff, 3, "%d,", flag);
	strcat(buff,flag_buff);
	strcat(buff,file);
	buff[buff_size-1] = '\0';

	//Get ready to error check
	errno = 0;
	h_errno = 0;

	//Attempt to send message to server
	bytes_sent = write(socket_fd,buff,strlen(buff)+1);

	//Data successfully sent
	if (bytes_sent >= 0) {

		memset(received_data,0,receive_data_size);
		received_data[receive_data_size-1] = '\0';

		bytes_received = read(socket_fd,received_data,receive_data_size-1);
		printf("response=%s\n",received_data);
		status = received_data[0];

		//Data successfully received
	    if (bytes_received >= 0) {

	    	//Receive message with format: "S,FILE_DESCRIPTOR" or "F,ERRNO,H_ERRNO"
	    	//STATUS: F = fail, S = success

	    	if (status == 'S') {

	    		file_desc = atoi(&received_data[2]);

	    	} else if (status == 'F') {

	    		char * part = strtok(received_data, ",");
	    		
	    		part = strtok(NULL, ",");
	    		errno = atoi(part);
	    		
	    		part = strtok(NULL, ",");
				h_errno = atoi(part);
	    		
	    		//printf("errno=%d,h_errno=%d\n",errno,h_errno);
	    	} else {
	    		fprintf(stderr, "Error: netopen() invalid response from server.\n");
	    		return -1;
	    	}

	    //Data not received
	    } else {

	    	fprintf(stderr, "Error: netopen() failed to receive data from server.\n");
	    
	    }
	//Data failed to send
	} else {

		fprintf(stderr, "Error: netopen() failed to send data to the server.\n");
	
	}       

	if (file_desc > -1){
		
		return file_desc;

	} else {
		//fails, return -1
		perror("netopen() failed");
		return -1;

	}
}

/**
    Used to read from a file on the server.

    file_desc: 	The server file descriptor of the file to read
   	buff: 		Buffer to store read data
   	nbyte: 		Number of bytes to read

    returns: 	A non-negative integer indicating the number of bytes actually read, or -1 if an error occurred
*/
ssize_t netread(int file_desc, void * buff, size_t nbyte){
	
	//Get ready to error check
	errno = 0;
	h_errno = 0;

	int bytes_read = 1;

	if (bytes_read > -1){
		
		return bytes_read;

	} else {
		//fails, set errno and return -1
		perror("netread() failed");
		return -1;	

	}
}

/**
    Used to write to a file on the server.

    file_desc: 	The server file descriptor of the file to write to
   	buff: 		Buffer to write from
   	nbyte: 		Number of bytes to write

    returns: 	A non-negative integer indicating the number of bytes actually written, or -1 if an error occurred
*/
ssize_t netwrite(int file_desc, const void * buff, size_t nbyte){
	
	//Get ready to error check
	errno = 0;
	h_errno = 0;
	
	int bytes_writ = 1;
	if (bytes_writ > -1){
	
		return bytes_writ;

	} else {
		//fails, set errno and return -1
		perror("netwrite() failed");
		return -1;

	}
}

/**
    Used to close a file on the server.

    file_desc: 	The server file descriptor of the file to close

    returns: 	0 if successful, or -1 if an error occurred
*/
int netclose(int file_desc) {
	int file_desc_length = snprintf(NULL, 0, "%d", file_desc);
	int buff_size = 1 + 2 + file_desc_length;//nullterm + 'operation,' + file_descriptor
	char buff[buff_size];
	int receive_data_size = 32;
	char received_data[receive_data_size];
	int bytes_sent, bytes_received;
	char status = 'F'; //fail by default

	if (socket_fd == -1) {
		fprintf(stderr,"Error: Connection does not exist.");
		return -1;
	}

	//Build message with format: "OPERATION,FLAG,FILENAME"
	memset(&buff,0,buff_size);
	strcat(buff,"1,"); //1 = close
	char file_desc_buff[file_desc_length+1];
	snprintf(file_desc_buff, file_desc_length+1, "%d,", file_desc);
	strcat(buff,file_desc_buff);
	buff[buff_size-1] = '\0';

	//Get ready to error check
	errno = 0;
	h_errno = 0;
	
	//Attempt to send message to server
	bytes_sent = write(socket_fd,buff,strlen(buff)+1);

	//Data successfully sent
	if (bytes_sent >= 0) {

		memset(received_data,0,receive_data_size);
		received_data[receive_data_size-1] = '\0';

		bytes_received = read(socket_fd,received_data,receive_data_size-1);
		printf("response=|%s|\n",received_data);
		status = received_data[0];
		printf("status=%c\n",status);

		//Data successfully received
	    if (bytes_received >= 0) {

	    	//Receive message with format: "S" or "F,ERRNO,H_ERRNO"
	    	//STATUS: F = fail, S = success

	    	if (status == 'S') {

	    		file_desc = atoi(&received_data[2]);

	    	} else if (status == 'F') {

	    		char * part = strtok(received_data, ",");
	    		
	    		part = strtok(NULL, ",");
	    		errno = atoi(part);
	    		
	    		part = strtok(NULL, ",");
				h_errno = atoi(part);
	    		
	    		//printf("errno=%d,h_errno=%d\n",errno,h_errno);
	    	} else {
	    		fprintf(stderr, "Error: netclose() invalid response from server.\n");
	    		return -1;
	    	}

	    //Data not received
	    } else {

	    	fprintf(stderr, "Error: netclose() failed to receive data from server.\n");
	    
	    }
	//Data failed to send
	} else {

		fprintf(stderr, "Error: netclose() failed to send data to the server.\n");
	
	}

	if (status == 'F'){
		//fails, set errno and return -1
		perror("netclose() failed");
		return -1;

	} else {
		
		return 0;

	}
}

/**
    Used to initialize connection to server and open socket.

    hostname: 	The name or IP of the server to connect to
    mode:		"unrestricted" | "exclusive" | "transaction"

    returns: 	0 if successful, or -1 if an error occurred
*/
int netserverinit(char * hostname, int mode){
	int connect_resp;
	struct addrinfo hints, * infoptr, * curr;

	memset(&hints, 0, sizeof(hints));//Zero out hints
	hints.ai_family = AF_INET; // IPv4 Address only
	hints.ai_socktype = SOCK_STREAM;

	int status = getaddrinfo(hostname, NULL, &hints, &infoptr); //DNS Lookup
	h_errno = 0;

	//Success
	if (status == 0) {

		//Create socket
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);

		if (socket_fd < 0) {
			perror("Error opening socket");
			exit(EXIT_FAILURE);
		}

		//Loop through attempt to create connection
		for (curr = infoptr;curr!=NULL;curr=curr->ai_next) {

			((struct sockaddr_in *)curr->ai_addr)->sin_port = htons(PORT_NUM);
			((struct sockaddr_in *)curr->ai_addr)->sin_family = AF_INET;

			connect_resp = connect(socket_fd,(struct sockaddr *) curr->ai_addr,curr->ai_addrlen);
			
			if (connect_resp == 0) {
				break;
			}
		}
	
		if (connect_resp < 0) {
			perror("Error connecting");
			exit(EXIT_FAILURE);
		}
		
		//At this point ready to run other functions.
		
		freeaddrinfo(infoptr); //free memory from getaddrinfo
		return 0;

	//Failure
	} else {
		h_errno = HOST_NOT_FOUND;
		freeaddrinfo(infoptr); //free memory from getaddrinfo
		return -1;

	}
}

int netserverclose() {
	if (socket_fd > -1) {
		close(socket_fd);
		socket_fd = -1;
		return 0;
	} else {
		fprintf(stderr,"Error: Socket already closed or never created.\n");
		return -1;
	}
}