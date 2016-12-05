#include "libnetfiles.h"

struct addrinfo hints, * infoptr;

int netopen(const char * file, int flags){
	
	int file_desc = open(file, flags);

	if (file_desc > -1){
		
		return file_desc;
	} else {
		//fails, set errno and return -1
		return -1;
	}
}

ssize_t netread(int file_desc, void * buff, size_t nbyte){
	
	ssize_t bytes_read = read(file_desc, buff, nbyte);

	if (bytes_read > -1){
		
		return bytes_read;
	} else {
		//fails, set errno and return -1
		return -1;	
	}
}

ssize_t netwrite(int file_desc, const void *buff, size_t nbyte){
	
	int bytes_writ = write(file_desc, buff, nbyte);

	if (bytes_writ > -1){
	
		return bytes_writ;
	} else {
		//fails, set errno and return -1
		return -1;
	}
}

int netclose(int fd){
	
	int result = close(fd);

	if (result == -1){
		//fails, set errno and return -1
		return -1;
	} else {
		
		return 0;
	}
}

int netserverinit(char * hostname, int mode){
	hints.ai_family = AF_INET; // IPv4 Address only
	int status = getaddrinfo(hostname, NULL, &hints, &infoptr);

	struct addrinfo * curr;
	char host[256], service[256];

	if (status == 0) {
		//success

		for (curr = infoptr;curr!=NULL;curr=curr->ai_next) {
			getnameinfo(curr->ai_addr, curr->ai_addrlen, host, sizeof(host), service, sizeof(service), NI_NUMERICHOST);
		}

		freeaddrinfo(infoptr); //free memory from getnameinfo
		return 0;
	} else {
		//failure
		return -1;
	}
}