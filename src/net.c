/*
 * net.c
 *
 *  Created on: 28.08.2014
 *      Author: veni
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>

#define DEST_PORT 58913

int sendDataToPCFarm(void *data, uint16_t size){
	int sockfd,n;
	struct sockaddr_in servaddr,cliaddr;
	sockfd=socket(AF_INET,SOCK_DGRAM,0);

	bzero(&servaddr,sizeof(servaddr) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=inet_addr("128.141.24.170");
	servaddr.sin_port = htons(DEST_PORT);

    sendto(sockfd,data, size, 0 , (struct sockaddr *) &servaddr, sizeof(servaddr));

	return size;
}



