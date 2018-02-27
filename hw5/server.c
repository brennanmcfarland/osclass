// Chris Fietkiewicz. Demonstrates C sockets. Designed to work with client.c.
// Usage: server port
// Example: server 8000
// Based on socket example from http://www.linuxhowtos.org/C_C++/socket.htm

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


char buffer[256];
char *question = "Why did the intern delete the repository?";
char *answer = "because he didn't git it";

// Helper function to conveniently print to stderr AND exit (terminate)
void error(const char *msg) {
    perror(msg);
    exit(1);
}

void read_from_client(int sockfd) {
  bzero(buffer, sizeof(buffer));
  int n = read(sockfd, buffer, sizeof(buffer));
  if (n < 0)
       error("ERROR reading from socket");
}

void write_to_client(char *message, int sockfd) {
  sprintf(buffer, message);
  int n = write(sockfd, buffer, sizeof(buffer));
  if (n < 0)
       error("ERROR writing to socket");
}

void ask_question(int sockfd) {
  write_to_client(question, sockfd);
  printf("Listening for client...\n");
  read_from_client(sockfd);
  buffer[strlen(answer)] = 0;
  if(strcmp(buffer, answer) == 0) {
    printf("Client was correct.\n");
    write_to_client("Correct!", sockfd);
    write_to_client(answer, sockfd);
    write_to_client("Hooray!\n", sockfd);
  }
  else {
    printf("Client was incorrect.\n");
    write_to_client("Incorrect! ", sockfd);
    write_to_client(answer, sockfd);
    write_to_client("... and neither did you.  You're stupid.\n", sockfd);
  }
}

int main(int argc, char *argv[]) {
     // Check for proper number of commandline arguments
     // Expect program name in argv[0], port # in argv[1]
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

     // Setup phase
     int sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     struct sockaddr_in serv_addr; // Server address struct
     bzero((char *) &serv_addr, sizeof(serv_addr));
     int portno = atoi(argv[1]); // Port number is commandline argument
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
          error("ERROR on binding");
     listen(sockfd, 5);
     // Service phase
     struct sockaddr_in cli_addr;
     socklen_t clilen = sizeof(cli_addr); // Address struct length
     int newsockfd = accept(sockfd,
                     (struct sockaddr *) &cli_addr,
                     &clilen);
     if (newsockfd < 0)
          error("ERROR on accept");
     ask_question(newsockfd);
     //read_from_client(newsockfd);
     //printf("Here is the message: %s\n", buffer);
     //write_to_client("I got your message", newsockfd);
     close(newsockfd);
     close(sockfd);
     return 0;
}
