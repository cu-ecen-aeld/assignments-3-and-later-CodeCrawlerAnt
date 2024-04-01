#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdbool.h>
#include <signal.h>
#include <arpa/inet.h>

#define BUFFSIZE 1024

bool signalreceived = false;

void signalhandler(int signalnumber)
{
  if(signalnumber == SIGINT)
  {
    signalreceived = true;
  }
  if(signalnumber == SIGTERM)
  {
    signalreceived = true;
  }
}


int main(int argc, char* argv[])
{
  int sockfd,clientsocket;
  struct addrinfo hints;
  struct addrinfo* res;
  struct sockaddr_in client_address;
  unsigned int client_length;
  char buffer[BUFFSIZE];
  struct sigaction new_action;

  openlog("aesdsocket", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);

  syslog(LOG_ERR, "starting app\n");

  memset(&new_action, 0, sizeof(struct sigaction));
  new_action.sa_handler = signalhandler;

  if(sigaction(SIGTERM, &new_action, NULL) != 0)
  {
    syslog(LOG_ERR, "error registering SIGTERM\n");
		closelog();
		return -1;  
  }

  if(sigaction(SIGINT, &new_action, NULL) != 0)
  {
    syslog(LOG_ERR, "error registering SIGINT\n");
		closelog();
		return -1;  
  }

  int fd = open("/var/tmp/aesdsocketdata", O_RDWR | O_APPEND | O_CREAT, 0666);
	if(fd == -1)
	{
		syslog(LOG_ERR, "File cannot be opened");
		closelog();
		return -1;
	}

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int s = getaddrinfo(NULL, "9000", &hints, &res);
  if(s != 0)
  {
    syslog(LOG_ERR, "getaddrinfo: %s\n", gai_strerror(s));
    closelog();
    return -1;
  }

  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if(sockfd < 0)
  {
    syslog(LOG_ERR, "failed to create a socket %d\n", sockfd);
    closelog();
    return -1;
  }

  if(bind(sockfd, res->ai_addr, res->ai_addrlen) < 0)
  {
    syslog(LOG_ERR, "failed to bind\n");
    closelog();
    return -1;
  }

  if(argc == 2 && strcmp(argv[1],"-d") != 0)
  {
    fork();
  }

  if(listen(sockfd, 5) < 0)
  {
    syslog(LOG_ERR, "failed to listen\n");
    closelog();
    return -1;
  }

  client_length = sizeof(client_address);
  
  while(true)
  {
     char* writebufptr = NULL;
     int totallength = 0;
     if ((clientsocket = accept(sockfd, (struct sockaddr *) &client_address, &client_length)) < 0) 
     {
        syslog(LOG_ERR, "accept failed\n");
        closelog();
        close(sockfd);
        close(clientsocket);
        close(fd);
        remove("/var/tmp/aesdsocketdata");
        return -1;
     }
     syslog(LOG_DEBUG, "Accepted connection from %s\n", inet_ntoa(client_address.sin_addr)); 
     int msg_size = 0;
     do 
     {
        msg_size = recv(clientsocket, buffer, BUFFSIZE, 0);
        if( msg_size <= 0)
        {
            syslog(LOG_DEBUG, "Closed connection from %s\n", inet_ntoa(client_address.sin_addr));
            break;
        }
        if(strchr(buffer, '\n') != NULL)
        {
          if(writebufptr == NULL)
          {
            writebufptr = buffer;
            totallength += msg_size;
          }
          else
          {
            writebufptr = (char*)realloc(writebufptr, totallength+msg_size);
            if(writebufptr != NULL)
            {
              memcpy(writebufptr+totallength, buffer, msg_size);
              totallength += msg_size;
            }
            else
            {
              syslog(LOG_ERR, "memory re allocation failed\n");
		          closelog();
		          return -1;
            }
          }
          int ret = write(fd, writebufptr, totallength);
	        if(ret < 0)
	        {
		        syslog(LOG_ERR, "file write failed\n");
		        closelog();
		        return -1;
	        }
          /*Send all the contents of the file*/
          lseek(fd,0, SEEK_SET);
          int readlen = 0;
          readlen = read(fd, buffer, BUFFSIZE);
          while(readlen > 0)
          {
            if(send(clientsocket, buffer, readlen, 0) < 0)
            {
              syslog(LOG_ERR, "send failed\n");
		          closelog();
		          return -1;
            }
            readlen = read(fd, buffer, BUFFSIZE);
          }
        }
        else 
        {
          if(writebufptr == NULL)
          {
            writebufptr = (char*)malloc(msg_size);
            if(writebufptr == NULL) 
            {
              syslog(LOG_ERR, "memory allocation failed\n");
		          closelog();
		          return -1;
            }
            memcpy(writebufptr,buffer,msg_size);
            totallength += msg_size;
          }
          else
          {
            writebufptr = (char*)realloc(writebufptr, totallength+msg_size);
            if(writebufptr != NULL)
            {
              memcpy(writebufptr+totallength, buffer, msg_size);
              totallength += msg_size;
            }
            else
            {
              syslog(LOG_ERR, "memory re allocation failed\n");
		          closelog();
		          return -1;
            }
          }
        }
        if(signalreceived)
        {
          break;
        }
     } while(msg_size > 0);
     if(signalreceived)
     {
        printf("Signal received");
        closelog();
        close(sockfd);
        close(clientsocket);
        close(fd);
        remove("/var/tmp/aesdsocketdata");
        break;
     }
     if(writebufptr != buffer)
     {
      free(writebufptr);
     }
  }

  return 0;


}