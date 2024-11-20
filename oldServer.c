#include "server.h"
#include "gameConfig.h"
#include "raylib.h"
#include "raymath.h"
#include "transport.h"
#include <ctype.h>
#include <netdb.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_SOCKET_CONNECTIONS 5

PlayerConnection **playerConnections;
GameState *gameState;

// Player movement commands
int forward = 0b01000;
int backward = 0b00100;
int left = 0b00010;
int right = 0b00001;

// thread function for new connecting clients
// Function needs to be passed to a spawn thread, constantly recvs player input,
// handles the physics, and responds with player position data for all players
// connected
void *handleClientConnection(void *args) {
  /*// cast to the struct that new need, read the socket from the struct*/
  /*ClientConnectionData *clientData = (ClientConnectionData *)args;*/
  /*PlayerInit playerInit = initializePlayerConnection();*/
  /*printf("Initialzed player connection\n");*/
  /*if (playerInit.validToJoin == 0b0) {*/
  /*  printf("Error joining\n");*/
  /*  return NULL;*/
  /*}*/
  /*printf("Player valid to join\n");*/
  /**/
  /*int socketFd = clientData->socketFd;*/
  /*Vector2 StartPos = playerInit.startPos;*/
  /**/
  /*Player *player = malloc(sizeof(Player));*/
  /*players[playerInit.playerId] = player;*/
  /*// ready to now accept input from the client, recvs data of type
   * playerInput*/
  /*// struct, and handles physics*/
  /**/
  /*// Player connection data*/
  /*char connected = 1;*/
  /*PlayerInput playerInput;*/
  /*/*printf("Player id before\n");*/
  /*int playerId = playerInit.playerId;*/
  /*/*printf("Player id after\n");*/
  /*// Return player position to be rendered on client side*/
  /*PlayerPosition playerPosition;*/
  /*Vector2 vel = (Vector2){0, 0};*/

  printf("Entering connected loop\n");

  while (1) {
    /*  recv(socketFd, &playerInput, sizeof(playerInput), 0);*/
    /*  printf("Player input recved\n");*/
    /*  uint8_t input = playerInput.input;*/
    /*  float rotationInRadians = player->rot * (M_PI / 180.0f);*/
    /*  printf("Taking right and left input\n");*/
    /*  if (input & right) {*/
    /*    // apply right velocity*/
    /*    player->rot += PLAYER_ROTATION_SPEED;*/
    /*  }*/
    /*  if (input & left) {*/
    /*    // apply left velocity*/
    /*    player->rot -= PLAYER_ROTATION_SPEED;*/
    /*  }*/
    /*  // take in player input data,*/
    /*  // mask bits to figure out which input is currently pressed*/
    /*  Vector2 direction = (Vector2){*/
    /*      cos(rotationInRadians),*/
    /*      sin(rotationInRadians),*/
    /*  };*/
    /*  player->rot = fmodf(player->rot + 360.0f, 360.0f);*/
    /*  printf("Input: %d\n", input);*/
    /**/
    /*  if (input & forward) {*/
    /*    // apply forward velocity*/
    /*    printf("Forward\n");*/
    /*    player->vel.x += direction.x * PLAYER_FORWARD_SPEED;*/
    /*    player->vel.y += direction.y * PLAYER_FORWARD_SPEED;*/
    /*  }*/
    /*  if (input & backward) {*/
    /*    // apply backward velocity*/
    /*    player->vel.x -= direction.x * PLAYER_FORWARD_SPEED;*/
    /*    player->vel.y -= direction.y * PLAYER_FORWARD_SPEED;*/
    /*  }*/
    /**/
    /*  // Update player position*/
    /*  player->pos.x += player->vel.x;*/
    /*  player->pos.y += player->vel.y;*/
    /**/
    /*  // should return object here*/
    /**/
    /*  // dampen*/
    /*}*/
    /**/
    /*return NULL;*/
  }
}

/* Function for client to connect, function for server to start */

int serverAccept(const char *host, int port) {
  // Check to see what IP and port it should be initiated on

  // Create a socket
  // Create addrinfo struct for current, next, and errors (next can be added
  // if there are issues with the connections)
  struct addrinfo *addr, hints;
  memset(&hints, 0, sizeof(hints));

  // set options
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  // get address info to pass into the socket
  //  convert the port to a string
  char portAsString[6];
  snprintf(portAsString, sizeof(portAsString), "%d", port);

  if (getaddrinfo(host, portAsString, &hints, &addr) < 0) {
    perror("Error on getAddrInfo");
    return 1;
  }

  int socketFd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
  if (socketFd < 0) {
    perror("Error creating socket");
    return 1;
  }

  // set socket option to reuse ports, this could change if we have multiple
  // servers coming up
  int option = 1;
  if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) <
      0) {
    perror("Error setting sock option");
    return 1;
  }
  if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEPORT, &option, sizeof(option)) <
      0) {
    perror("Error setting sock option");
    return 1;
  }

  // socket option set, bind to port now
  if (bind(socketFd, addr->ai_addr, addr->ai_addrlen) < 0) {
    perror("Error binding");
    return 1;
  }

  if (listen(socketFd, MAX_SOCKET_CONNECTIONS) != 0) {
    perror("Error on listen");
    return 1;
  }

  printf("Socket set up success, awaiting connectionss\n");

  // begin main loop of accpeting client connections
  int connectingSocket;
  struct sockaddr connectingAddr;
  socklen_t connectingAddrLen = sizeof(connectingAddr);
  GamePacket recvPacket;
  GamePacket sendPacket;

  int numPlayers = 0;
  while (1) {
    connectingSocket = accept(socketFd, &connectingAddr, &connectingAddrLen);
    if (connectingSocket < 0) {
      perror("Error on accept call");
      return -1;
    }
    recv(connectingSocket, &recvPacket, sizeof(recvPacket), 0);
    if (recvPacket.type == PACKET_JOIN_REQUEST) {
      // Check if player is valid to join, and game is not ongoing
      if (numPlayers < MAX_PLAYERS && gameState->gameState == GAME_WAITING) {
        // Player is valid to join Player id would be numPlayers
        // Store player's name, colors, send back id
        sendPacket.type = PACKET_GAME_STATUS;
        sendPacket.data.gameStatus.canJoin = 0b1;
        // Add player to the list of players

      } else if (gameState->gameState == GAME_ONGOING) {
        sendPacket.type = PACKET_GAME_STATUS;
        sendPacket.data.gameStatus.canJoin = 0b0;
        sendPacket.data.gameStatus.gameStarted = 0b1;
        printf("Cannot connect to ongoing game\n");
        return -1;
      } else {
        sendPacket.type = PACKET_GAME_STATUS;
        sendPacket.data.gameStatus.canJoin = 0b0;
        printf("Cannot connect to game\n");
        return -1;
      }
    }

    // As player joins, add to list of players, each will need their own
    // worker thread

    /*printf("Creating client connection struct\n");*/
    /*ClientConnectionData *clientData = calloc(1,
     * sizeof(ClientConnectionData));*/
    /*if (clientData == NULL) {*/
    /*  printf("Error creating client data struct\n");*/
    /*  return 1;*/
    /*}*/
    /*clientData->socketFd = connectingSocket;*/
    /*clientData->playerId = numPlayers++;*/
    /*printf("Client connection struct created\n");*/
    /*pthread_t thread_id;*/
    /*pthread_create(&thread_id, NULL, handleClientConnection, clientData);*/
    /*pthread_join(thread_id, NULL);*/
    /*printf("Successfully created thread for client: id:%d",*/
    /*       clientData->playerId);*/
    /**/
    /*// Send out array of player data*/
    /*printf("Sending out player data\n");*/
    /*send(connectingSocket, players, sizeof(players), 0);*/
  }

  // done with connections, can close socket
  close(socketFd);
}

int setupServer() {
  /*char host[256];*/
  /*int port;*/
  /*printf("\nServer: Enter Server IP: ");*/
  /*scanf("%s", host);*/
  /**/
  /*printf("\nServer: Enter Server Port: ");*/
  /*scanf("%d", &port);*/
  char host[] = "localhost";
  int port = 9876;
  // init the list of players;
  players = malloc(sizeof(Player *) * MAX_PLAYERS);
  // memset to make sure they are null
  memset(players, 0, sizeof(Player *) * MAX_PLAYERS);
  // init the game state, set it globally
  gameState = malloc(sizeof(GameState));

  gameState->gameState = GAME_WAITING;
  int ret = serverAccept(host, port);
  if (ret == 0) {
    printf("Server creation successfull\n");
  }
  // create the broadcast thread that sends out the player updates for clients
  // to ingest

  return 0;
}

/*int main(void) {*/
/*  setupServer();*/
/**/
/*  return 0;*/
/*}*/
