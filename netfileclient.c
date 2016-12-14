#include "libnetfiles.h"

int main(int argc, char * argv[]){
	
	int res = netserverinit("factory.cs.rutgers.edu",0);
	if (res == 0) {
		//successful connection
		printf("Connection successful!\n");
	}
	char buff[500];
	memset(buff,0,500);
	buff[499] = '\0';

	//int y = 1;
	int x = 3500;
	int y;
	//x = 30000;

	int fd = netopen("dummy.txt", O_RDWR);
	//int fd2 = netopen("dummdy.txt", O_RDWR);
	//netread(fd, buff, 18);
	//netwrite(fd, &x,sizeof(int));
	netread(fd, &y,sizeof(int));
	printf("y=%d",y);
	//printf("buff=%s\n",buff);

	//netwrite(fd, "hiya", 5);
	
	//int pause;
	//scanf("%d",&pause);


	//netread(fd, buff, 20);
	//printf("buff=%s\n",buff);

	netclose(fd);
	
	return 0;
}