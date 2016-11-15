#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <fcntl.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    FILE *file; 
    FILE *file2; 
    int fp, fp2;
    int bytesread, length, thisPort;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int c, i, j;

    const int BUFFERSIZE = 1000;
    char buffer[BUFFERSIZE+1];
    char message[BUFFERSIZE+1];
    char header[10];
    char carrot[3] = "> ";
    char quit[6] = "\\quit";
    
    if (argc != 3) {
        fprintf(stderr,"usage: client hostname portnumber\n");
        exit(1);
    }
    
    // GET HANDLE FROM USER
    char handle[20];
    printf("Input Handle: ");
    fgets(handle,20,stdin);
    handle[strlen(handle)-1] = 0; // null terminate
    strcat(handle,carrot);
        
    // ESTABLISH CONNECTION
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    while(1) {
      // SEND TEXT
      bzero(buffer,BUFFERSIZE+1);
      bzero(message,BUFFERSIZE+1);
      
      printf("%s",handle);
      fgets(buffer,BUFFERSIZE,stdin); // get message from user
      buffer[strlen(buffer)-1] = 0; // remove return character

      strcpy(message,handle);
      strcat(message,buffer); // add handle to our input buffer to make:
      n = write(sockfd,message,strlen(message)); // message to send
      if (n < 0) error("ERROR writing to socket");
      if (strcmp(buffer, quit) == 0) break;
      
      // RECEIVE TEXT
      bzero(buffer,BUFFERSIZE+1);
      
      n = read(sockfd,buffer,BUFFERSIZE+1);
      printf("%s\n",buffer);
      if (strcmp(buffer, quit) == 0) break;
    }
      
    
    close(sockfd);
    printf("Connection to server terminated.\n");
    return 0;
}
