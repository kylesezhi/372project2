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

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int listenForClient(int portno, struct sockaddr_in *cli_addr, socklen_t *clilen) {
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

int initiateConnection(int sockfd, struct sockaddr_in *cli_addr, socklen_t *clilen) {
  int newsockfd = accept(sockfd, 
              (struct sockaddr *) cli_addr, 
              clilen);
  if (newsockfd < 0) error("ERROR on accept");
  
  return newsockfd;
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, i;
     
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
     
     sockfd = listenForClient(atoi(argv[1]), &cli_addr, &clilen);

     while(1) { // MAIN GAME LOOP lolz
       printf("Waiting for connection\n");
       
       newsockfd = initiateConnection(sockfd, &cli_addr, &clilen);
      
      // GET COMMAND
      bzero(buffer,30);
      n = read(newsockfd,buffer,30); // get the first command
      string = buffer;
      i = 0;
      while( (found = strsep(&string," ")) != NULL ) {
        strcpy(command[i], found);
        // printf("%s\n",found);
        i++;
      }
      // printf("%s/%s/%s\n",command[0],command[1],command[2]);
      if(strcmp(command[0], "-l") == 0) {
        printf("list\n");
      } else if (strcmp(command[0], "-g") == 0) {
        printf("get %s\n", command[1]);
      } else {
        printf("huh?\n");
        
      }

      // GET OK
      // bzero(buffer,10);
      // bzero(header,10);
      // n = read(newsockfd,header,4); // get the header
      // n = read(newsockfd,buffer,4); // get the body
      
      close(newsockfd);


    } // end while(1)
            
    //  close(newsockfd);
  //  close(sockfd);
     
     return 0; 
}
