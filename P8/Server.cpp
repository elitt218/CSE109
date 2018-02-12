/*
  CSE109
  Lizzie Litt
  esl218
  Program Description: network server that handles requests
  Progam #8
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<errno.h>

void handleError(int errorValue);

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		fprintf(stderr, "Missing argument, or have too many.\n");
		return 1;
	}
	//save filename
	FILE *fname = fopen("file.txt", "w");
	fprintf(fname, "%s", argv[1]);
	fclose(fname);
	
	//make socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		handleError(sock);
		fprintf(stderr, "Can't create socket.\n");
		return 1;
	}

	int listenPort = 10000;
	while(listenPort < 10500)
	{
		//fill in address struct
		struct sockaddr_in myaddr;
		memset(&myaddr, 0, sizeof(struct sockaddr_in));
		myaddr.sin_family = AF_INET;
		myaddr.sin_port = htons(listenPort);
		myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
		//bind socket to address
		int bindResult = bind(sock, (struct sockaddr*) &myaddr, sizeof(myaddr));
		if(bindResult < 0)
		{
			handleError(bindResult);
			fprintf(stderr,"Can't bind socket.\n");
			listenPort++;
			continue;
		}

		struct linger linger_opt = {1,0}; //set linger timeout to 0
		setsockopt(sock, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt));
		
		//listen for connection
		bindResult = listen(sock, 1);
		if(bindResult < 0)
		{
			handleError(bindResult);
			fprintf(stderr,"Can't listen.\n");
			listenPort++;
			continue;
		}
		break;
	}

	//save port number
	FILE *pNum = fopen("port.txt", "w+");
	fprintf(pNum, "%d", listenPort);
	if(NULL)
	{
		fprintf(stderr, "Errno(%d): %s\n", errno, strerror(errno));;
	}
	fclose(pNum);

	while(true)
	{
		//accept connection
		struct sockaddr_in peeraddr;
		memset(&peeraddr, 0, sizeof(struct sockaddr_in));
		socklen_t peeraddr_len;
		memset(&peeraddr_len, 0, sizeof(socklen_t));
		int connect = accept(sock, (struct sockaddr*) &peeraddr, &peeraddr_len);
		if(connect < 0)
		{
			handleError(connect);
			fprintf(stderr,"Can't accept.\n");
		}
				
		printf(
			"Connection from IP %d.%d.%d.%d, port %d\n",
			(ntohl(peeraddr.sin_addr.s_addr) >> 24) & 0xff, // High byte
			// of address
			(ntohl(peeraddr.sin_addr.s_addr) >> 16) & 0xff, // . . .
			(ntohl(peeraddr.sin_addr.s_addr) >> 8) & 0xff,  // . . .
			ntohl(peeraddr.sin_addr.s_addr) & 0xff,         // Low byte of
			// addr
			ntohs(peeraddr.sin_port)
			);
		
		char buffer[3];
		ssize_t readResult = read(connect, buffer, 2); //read 1st 2 bytes
		fprintf(stderr, "read: %d\n", (int)readResult);
		if (readResult < 0)
		{
			handleError(readResult);
			fprintf(stderr, "read error.\n");
		}
		unsigned short readRest = *(unsigned short*)&buffer; //get size
		fprintf(stderr, "read rest: %d.\n", readRest);
		
		char newBuff[readRest];
		int j = 0;
		for(j=0; j<readRest; j++)
		{
			newBuff[j] = 0;
		}
		ssize_t readAll = read(connect, newBuff, readRest);
		fprintf(stderr, "read all: %d.\n",(int) readAll);
		
		while(readAll < (readRest-2)) //make sure reading all
		{
			ssize_t readMore = read(connect, &newBuff[readAll], (readRest-2) - readAll);
			fprintf(stderr, "readMore: %d.\n", (int)readMore);
			if(readMore == -1)
			{
				fprintf(stderr, "reading more error. Errno(%d): %s\n", errno, strerror(errno));
				return 1;
			}
			if(readMore == 0)
			{
				fprintf(stderr, "Nothing left to read.\n");
				break;
			}
			readAll += readMore;
			fprintf(stderr, "now readAll: %d.\n", (int)readAll);
		}
		fprintf(stderr, "now read: %d.\n", (int)readAll);
		
		unsigned int startLoc = *(unsigned int*)&newBuff[0];
		fprintf(stderr, "startLoc: %d.\n", startLoc);
		unsigned int dataReq = *(unsigned int*)&newBuff[4];
		fprintf(stderr, "dataReq: %d.\n", dataReq);
		
		int i = 0;
		unsigned char checksum = 0;
		for(i=0; i < readRest; i++)
		{
			checksum += (unsigned char)newBuff[i];
		}
		//fprintf(stderr, "checksum: %u.\n", (unsigned int)checksum);
		
		int fd;
		handleError(fd = open(argv[1], O_RDWR)); //open file
		
		char theFile[dataReq];
		handleError(lseek(fd, startLoc, SEEK_SET));
		ssize_t readFile = read(fd, theFile, dataReq);
		handleError(readFile);
		
		unsigned char fileInfo[5];
		*(unsigned int*)&fileInfo[0] = readFile+5;
		*(unsigned char*)&fileInfo[4] = checksum;
		
		write(connect, fileInfo, 5);
		ssize_t writeData = write(connect, theFile, dataReq);
		fprintf(stderr, "writing data: %d.\n", (int)writeData);

		while(writeData < readFile)
		{
			ssize_t writeMore = write(connect, &theFile[writeData], readFile - writeData);
			handleError(writeMore);
			
			if(writeMore == 0)
			{
				fprintf(stderr, "Nothing was written.\n");
				break;
			}
			writeData += writeMore;
		}
		
		if(startLoc == 0 && dataReq == 0)
		{
			close(sock);
			return 1;
		}
	}
	
	close(sock);	
	return 0;
}



void handleError(int errorValue)
{
	if(errorValue == -1)
	{
		fprintf(stderr, "Errno(%d): %s\n", errno, strerror(errno));
		exit(1);
	}
}
