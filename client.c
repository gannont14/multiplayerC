
#include <ctype.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int clientConnect(const char *host, int port) {
  // use getaddrinfo to create addrinfo object,
  // also set a hints info object,
  // call the socket function, could put into loop for all the addresses in the
  // getaddrinfo
  // create addr for server, call connect
  struct addrinfo *addr, hints;
  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = 0;

  // call getaddrinfo
  // need to convert port to a string first
  char portAsString[6];
  snprintf(portAsString, sizeof(portAsString), "%d", port);

  if (getaddrinfo(host, portAsString, &hints, &addr) < 0) {
    perror("Client: getaddrinfo");
    return -1;
  }

  int socketFd;
  socketFd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
  if (socketFd < 0) {
    perror("Client: socket call");
    return -1;
  }

  int status;
  status = connect(socketFd, addr->ai_addr, addr->ai_addrlen);
  if (status < 0) {
    perror("Client: connect call");
    return -1;
  }

  freeaddrinfo(addr);

  /*// TODO : remove after testing*/
  /*char buffer[1024] = {0};*/
  /*char hello[] = "Hello World\n";*/
  /*int bytesRead, bytesSent;*/

  /*bytesSent = send(socketFd, hello, strlen(hello), 0);*/
  /*printf("Client: bytes sent: %d\n", bytesSent);*/
  /*printf("Client: hello message sent\n");*/
  /*bytesRead = recv(socketFd, buffer, 1024 - 1, 0);*/
  /*printf("Client: Num Bytes read: %d\n", bytesRead);*/
  /*buffer[bytesRead] = '\0';*/
  /*printf("Client: bytes read: \n%s\n", buffer);*/

  // send test data for the input

  return socketFd;
}

int setupClient() {
  /*char host[256];*/
  /*int port;*/
  /*printf("\nClient: Enter Server IP to connect to : ");*/
  /*scanf("%s", host);*/
  /**/
  /*printf("\nClient: Enter Server Port: ");*/
  /*scanf("%d", &port);*/

  char host[] = "localhost";
  int port = 9876;

  int ret = clientConnect(host, port);

  if (ret > 0) {
    printf("Client connection successfull\n");
  }
  return ret;
}

/*int main(void) {*/
/*  setupClient();*/
/*  return 0;*/
/*}*/
