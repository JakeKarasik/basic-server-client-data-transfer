#CC     =       cc -pthread
CC      =       gcc
FLAGS   =       -Wall -pthread -g
COMPILE =       $(CC) $(FLAGS)

all: server client

server: libnetfiles.c netfileserver.c libnetfiles.h
	$(COMPILE) libnetfiles.c netfileserver.c -o server

client: netfileclient.c libnetfiles.h
	$(COMPILE) netfileclient.c -o client
	
clean:
	rm -rf *.o server client