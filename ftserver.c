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
#include <dirent.h>

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

int clientConnect(char *host, int portnum) {
  struct sockaddr_in serv_addr;
  struct hostent *server;
  
  int sockfdone = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfdone < 0) 
      error("ERROR opening socket");
  server = gethostbyname(host);
  if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
  }
  
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portnum);
  // printf("DEBUG %s\n", server->h_addr);
  if (connect(sockfdone,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
      error("ERROR connecting");

  return sockfdone;
}

void sendListing(int datasockfd) {
  // hat tip to: http://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
  DIR *d;
  struct dirent *dir;
  char dirlist[1001];
  strcpy(dirlist, "");
  d = opendir(".");
  if (d) {
    while ((dir = readdir(d)) != NULL)
    {
      strcat(dirlist, dir->d_name);
      strcat(dirlist, "\n");
    }
    closedir(d);
  }
  write(datasockfd, dirlist, strlen(dirlist));
}

int validateFilename(char *filename) {
  DIR *d;
  struct dirent *dir;
  d = opendir(".");
  if (d) {
    while ((dir = readdir(d)) != NULL)
    {
      if(strcmp(filename, dir->d_name) == 0) return 1;
    }
    closedir(d);
  }
  return 0;
}

void getFile(ing datasockfd, char *filename) {
  file = fopen(filename, "r");
  if (file < 0) error("ERROR could not open file.\n");
  if (file == NULL) error("ERROR could not open file.\n");
  
  while((bytesread = fread(buffer, 1, BUFFERSIZE, file)) > 0) { 
    buffer[bytesread] = 0; // append with null
    n = write(datasockfd,buffer,strlen(buffer));
    if (n < 0) error("ERROR writing to socket");
  }
  fclose(file);
}

int main(int argc, char *argv[])
{
     int sockfd, controlsockfd, datasockfd, i, bytesread;
     
     const int BUFFERSIZE = 1000;
     char buffer[BUFFERSIZE+1];
     int fp, fp2;
     FILE *file;
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
       printf("Waiting for connection.\nPress Ctrl-C to exit.\n");
       controlsockfd = acceptConnect(sockfd, &cli_addr, &clilen);
       
       // EXTRACT NAME OF REMOTE HOST
       char host[1024];
       char service[20];
       getnameinfo((struct sockaddr *) &cli_addr, sizeof(cli_addr), host, sizeof(host), service, sizeof(service), 0);
      
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
      // LIST
      if(strcmp(command[0], "-l") == 0) { 
        // printf("DEBUG list\n");
        datasockfd = clientConnect(host, atoi(command[1]));
        sendListing(datasockfd);
                
      // GET [filename]
      } else if (strcmp(command[0], "-g") == 0) { 
        
        if(validateFilename(command[1])){
          write(controlsockfd, "OK", strlen("OK"));
          
          // printf("connect to %s on %d\n", host, atoi(command[2]));
          datasockfd = clientConnect(host, atoi(command[2]));
          
          // filename, datasockfd
          getFile(datasockfd, command[1]);
          // file = fopen(command[1], "r");
          // if (file < 0) error("ERROR could not open file.\n");
          // if (file == NULL) error("ERROR could not open file.\n");
          // 
          // while((bytesread = fread(buffer, 1, BUFFERSIZE, file)) > 0) { 
          //   buffer[bytesread] = 0; // append with null
          //   n = write(datasockfd,buffer,strlen(buffer));
          //   if (n < 0) error("ERROR writing to socket");
          // }
          // fclose(file);
          
        } else {
          write(controlsockfd, "Invalid filename.", strlen("Invalid filename."));
        }
        
      // INVALID COMMAND
      } else { 
        write(controlsockfd, "Invalid command.", strlen("Invalid command."));
      }
      close(datasockfd);
      close(controlsockfd);

    } // end while(1)
            
   close(sockfd); // TODO
     
     return 0; 
}
