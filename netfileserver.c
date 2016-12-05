#include "libnetfiles.h"

int main(int argc, char * argv[]){
	
	int res = netserverinit("factory.cs.rutgers.edu",0);
	printf("%s",res == 0 ? "Connection Successful!\n" : "Connection Failed!\n");

	return 0;
}