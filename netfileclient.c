#include "libnetfiles.h"

int main(int argc, char * argv[]){
	
	//int res = netserverinit("factory.cs.rutgers.edu",0);

	//if (res == 0) {
		//successful connection
		
	//}


	int socket_fil_des, numbytes;
	struct sockaddr_in serv_addr;
	struct hostent * server;
	char buffer[READ_SIZE];

	socket_fil_des = socket(AF_INET, SOCK_STREAM, 0);
	server = gethostbyname("factory.cs.rutgers.edu");
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	//strncpy(server->h_addr, serv_addr.sin_addr.s_addr,server->h_length);
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(PORT_NUM);
	connect(socket_fil_des,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
	printf("Please enter the message: ");
	memset(&buffer,0,READ_SIZE);
    fgets(buffer,READ_SIZE-1,stdin);
    numbytes = write(socket_fil_des,buffer,strlen(buffer));
    if (numbytes < 0) 
         perror("ERROR writing to socket");
    memset(&buffer,0,READ_SIZE);
    numbytes = read(socket_fil_des,buffer,READ_SIZE-1);
    if (numbytes < 0) 
         perror("ERROR reading from socket");
    printf("%s\n",buffer);
    close(socket_fil_des);
	return 0;
}