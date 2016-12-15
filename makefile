#CC     =       cc -pthread
CC      =       gcc
FLAGS   =       -Wall -g
COMPILE =       $(CC) $(FLAGS)

all: server client

server: netfileserver.c
	$(COMPILE) -pthread netfileserver.c -o server

client: libnetfiles.c libnetfiles.h
	$(COMPILE) libnetfiles.c -c
	
clean:
	rm -rf *.o server