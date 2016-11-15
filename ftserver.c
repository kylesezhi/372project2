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

// TODO
// make dec work

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     
     const int BUFFERSIZE = 1000;
     char buffer[BUFFERSIZE+1];
     char header[10];
     char ok[10];
     int fp, fp2;
     FILE *file;
     FILE *file2;
     int c, i, j, bytesread, thisPort, yes, x;
     pid_t spawnpid = -5;
     int randomPorts[8];
     int currentPort = -1;
     
     char temp_plaintext[13];
     char temp_key[13];
     char temp_cipher[13];
     
     struct sockaddr_in serv_addr, cli_addr;
     int n, length;
     if (argc != 2) {
         fprintf(stderr,"usage: ftserver portnumber\n");
         exit(1);
     }
          
     // ESTABLISH CONNECTION
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("1ERROR on binding");
     setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     

     while(1) { // MAIN GAME LOOP lolz
      //  printf("MAIN LOOP SERVER\n"); //TRACE
      currentPort++;
       
       newsockfd = accept(sockfd, 
                   (struct sockaddr *) &cli_addr, 
                   &clilen);
       if (newsockfd < 0) error("ERROR on accept");
            
      // GIVE RANDOM PORT
      bzero(buffer,BUFFERSIZE+1);
      snprintf(buffer, 10, "%d", randomPorts[currentPort]);
      n = write(newsockfd,buffer,5);
      if (n < 0) error("ERROR writing to socket");
      

      // GET OK
      // bzero(buffer,10);
      // bzero(header,10);
      // n = read(newsockfd,header,4); // get the header
      // n = read(newsockfd,buffer,4); // get the body
      
      // sleep(1); // FIXME wait for client to close, then we close
      close(newsockfd); //??? without it, it works on MacOS


    } // end while(1)
            
    //  close(newsockfd);
  //  close(sockfd);
     
     return 0; 
}
