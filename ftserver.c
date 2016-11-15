/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>

#include <errno.h>
#include <netdb.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int setupListen(int portno, struct sockaddr_in *cli_addr, socklen_t *clilen) {
  struct sockaddr_in serv_addr;
  int yes;

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
     error("ERROR opening socket");
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr,
           sizeof(serv_addr)) < 0) 
           error("ERROR on binding");
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  listen(sockfd,5);
  *clilen = sizeof(*cli_addr);
  
  return sockfd;
}

int acceptConnect(int sockfd, struct sockaddr_in *cli_addr, socklen_t *clilen) {
  int newsockfd = accept(sockfd, 
              (struct sockaddr *) cli_addr, 
              clilen);
  if (newsockfd < 0) error("ERROR on accept");
  
  return newsockfd;
}

int receiveMessage(char buffer[], int newsockfd) {
  bzero(buffer,30);
  int n = read(newsockfd,buffer,30); // get the first command

  return n;
}

int main(int argc, char *argv[])
{
     int sockfd, controlsockfd, datasockfd, i;
     
     const int BUFFERSIZE = 1000;
     char buffer[BUFFERSIZE+1];
     int fp, fp2;
     FILE *file;
     FILE *file2;
     char *found, *string;
     char command[3][BUFFERSIZE+1];
     
     int n, length;
     
     struct sockaddr_in cli_addr;
     socklen_t clilen;
     
     if (argc != 2) {
         fprintf(stderr,"usage: ftserver portnumber\n");
         exit(1);
     }
     
     sockfd = setupListen(atoi(argv[1]), &cli_addr, &clilen);

     while(1) { // MAIN GAME LOOP lolz
       printf("Waiting for connection\n");
       controlsockfd = acceptConnect(sockfd, &cli_addr, &clilen);
      
      // GET COMMAND
      n = receiveMessage(buffer, controlsockfd);
      
       // SPLIT COMMAND
      string = buffer;
      i = 0;
      while( (found = strsep(&string," ")) != NULL ) {
        strcpy(command[i], found);
        i++;
      }
      
      // EXECUTE COMMAND
      if(strcmp(command[0], "-l") == 0) { // list
        printf("list\n");
        
        struct addrinfo hints, *res;
        int datasockfd;

        // first, load up address structs with getaddrinfo():

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        getaddrinfo("localhost", command[1], &hints, &res);

        // make a socket:

        datasockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

        // connect!

        connect(datasockfd, res->ai_addr, res->ai_addrlen);
        n = write(datasockfd, "hi there", strlen("hi there"));
        
        // struct sockaddr_in client_addr;
        // socklen_t addrlen;
        // n = getpeername(controlsockfd, (struct sockaddr *) &client_addr, &addrlen);
        // if (n == -1) error("ERROR getpeername()");
        // client_addr.sin_port = htons(atoi(command[1]));
        // 
        // datasockfd = socket(AF_INET, SOCK_STREAM, 0);
        // // printf("datasocketfd: %d controlsockfd: %d\n", datasockfd, controlsockfd);
        // n = connect(datasockfd, (struct sockaddr *) &client_addr, addrlen);
        // if (n == -1) error("ERROR connect()");
        // n = write(datasockfd, "hi there", strlen("hi there"));
        
      } else if (strcmp(command[0], "-g") == 0) { // get [filename]
        printf("get %s\n", command[1]);
      } else { // invalid command
        printf("huh?\n");
        
      }
      
      close(controlsockfd);


    } // end while(1)
            
   close(sockfd); // TODO
     
     return 0; 
}
