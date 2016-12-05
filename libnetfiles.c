#include "libnetfiles.h"

int netopen(const char * pathname, int flags){
	
	int file_descr = open(pathname, flags);
	if(file_descrip > -1){
		
		return file_descr;
		
	}else{
		
		set_lib_errno(lib_errno, errno);
		return -1;
		
	}
	
}

ssize_t netread(int fildes, void * buf, size_t nbyte){
	
	ssize_t bytes_read = read(fildes, buf, nbyte);
	if(bytes_read > -1){
		
		return bytes_read;
		
	}else{
		
		set_lib_errno(lib_errno, errno);
		return -1;
		
	}
	
}

ssize_t netwrite(int fildes, const void *buf, size_t nbyte){
	
	int bytes_writ = write(fildes, buf, nbyte);
	if(bytes_writ > -1){
		
		return bytes_writ;
		
	}else{
		
		set_lib_errno(lib_errno, errno);
		return -1;
		
	}
	
}

int netclose(int fd){
	
	int close = close(fd);
	if(close == -1){
		
		set_lib_errno(lib_errno, errno);
		return close;
		
	}
	return close;
	
}

int netserverinit(char * hostname){
	
	
	return 0;
	
}