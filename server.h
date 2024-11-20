#include <stdio.h>

typedef struct {
  int socketFd;
  int playerId;
} ClientConnectionData;

int setupServer();
int serverAccept(const char *host, int port);
