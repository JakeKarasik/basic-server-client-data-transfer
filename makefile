#CC     =       cc
CC      =       gcc
FLAGS   =       -Wall -g
COMPILE =       $(CC) $(FLAGS)

all: compressT_LOLS compressR_LOLS tester

compressT_LOLS: compressT_LOLS.c functions.c compress.h
	$(COMPILE) -pthread -c functions.c compressT_LOLS.c

compressR_LOLS: compressR_LOLS.c compressR_worker_LOLS.c functions.c compress.h
	$(COMPILE) -c functions.c compressR_LOLS.c
	$(COMPILE) functions.c compressR_worker_LOLS.c -o Worker 

tester: tester.c functions.o compressT_LOLS.o compress.h
	$(COMPILE) -pthread tester.c functions.o compressT_LOLS.o compressR_LOLS.o -o tester
	
clean:
	rm -rf *.o Worker tester
