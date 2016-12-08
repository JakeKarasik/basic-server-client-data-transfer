#include "libnetfiles.h"

int main(int argc, char * argv[]){
	
	char * address = "factory.cs.rutgers.edu";
	int res = netserverinit(address,0);
	//printf("%s",res == 0 ? "Connection Successful!\n" : "Connection Failed!\n");

	if (res == 0) {
		//successful connection
		
	}

	return 0;
}