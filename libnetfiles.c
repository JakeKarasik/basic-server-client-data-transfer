#include "libnetfiles.h"

int netopen(const char * file, int flags){
	
	errno = 0;
	int file_desc = open(file, flags);

	if (file_desc > -1){
		
		return file_desc;

	} else {
		//fails, set errno and return -1
		perror("netopen failed:");
		return -1;

	}
}

ssize_t netread(int file_desc, void * buff, size_t nbyte){
	
	errno = 0;
	ssize_t bytes_read = read(file_desc, buff, nbyte);

	if (bytes_read > -1){
		
		return bytes_read;

	} else {
		//fails, set errno and return -1
		perror("netread failed:");
		return -1;	

	}
}

ssize_t netwrite(int file_desc, const void *buff, size_t nbyte){
	
	errno = 0;
	int bytes_writ = write(file_desc, buff, nbyte);

	if (bytes_writ > -1){
	
		return bytes_writ;

	} else {
		//fails, set errno and return -1
		perror("netwrite failed:");
		return -1;

	}
}

int netclose(int file_desc){
	
	errno = 0;
	int result = close(file_desc);

	if (result == -1){
		//fails, set errno and return -1
		perror("netclose failed:");
		return -1;

	} else {
		
		return 0;

	}
}

int netserverinit(char * hostname, int mode){
	struct addrinfo settings, * infoptr;
	settings.ai_family = AF_INET; // IPv4 Address only
	int status = getaddrinfo(hostname, NULL, &settings, &infoptr);
	struct addrinfo * curr;
	char host[256], service[256];
	h_errno = 0;

	if (status == 0) {
		//success

		for (curr = infoptr;curr!=NULL;curr=curr->ai_next) {
			getnameinfo(curr->ai_addr, curr->ai_addrlen, host, sizeof(host), service, sizeof(service), NI_NUMERICHOST);
		}

		freeaddrinfo(infoptr); //free memory from getnameinfo
		return 0;

	} else {
		//failure
		h_errno = HOST_NOT_FOUND;
		return -1;

	}
}