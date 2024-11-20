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
#define NUM_PLAYERS_TO_START 2

PlayerConnection **playerConnections;
Player **players;
GameState *gameState;

// Player movement commands
int forward = 0b01000;
int backward = 0b00100;
int left = 0b00010;
int right = 0b00001;

void serverStartGame();
void *handleClientConnection(void *args) { return NULL; }

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

  printf("Server: Socket set up success, awaiting connectionss\n");

  // begin main loop of accpeting client connections
  int connectingSocket;
  struct sockaddr connectingAddr;
  socklen_t connectingAddrLen = sizeof(connectingAddr);
  GamePacket recvPacket;
  GamePacket sendPacket;
  PlayerConnection *playerConnection;
  Player *player;
  int playerId;
  int bytesRecvd;

  int numPlayers = 0;
  printf("Server: Entering connection loop\n");
  while (1) {
    connectingSocket = accept(socketFd, &connectingAddr, &connectingAddrLen);
    printf("Server: Connecting socket accepted\n");
    if (connectingSocket < 0) {
      perror("Error on accept call");
      return -1;
    }

    bytesRecvd = recv(connectingSocket, &recvPacket, sizeof(recvPacket), 0);
    printf("Server: Bytes recved: %d\n", bytesRecvd);
    printf("Server: Packet is of type %d\n", recvPacket.type);

    if (recvPacket.type == PACKET_JOIN_REQUEST) {
      printf("Server: Packet is of type join request\n");
      // Check if player is valid to join, and game is not ongoing
      if (numPlayers < MAX_PLAYERS && gameState->gameState == GAME_WAITING) {
        printf("Server: Player is able to join\n");
        // Player is valid to join Player id would be numPlayers
        // Store player's name, colors, send back id
        sendPacket.type = PACKET_GAME_STATUS;
        sendPacket.data.gameStatus.canJoin = 0b1;
        printf("Server: Created send packet\n");
        send(connectingSocket, &sendPacket, sizeof(sendPacket), 0);
        printf("Server: sent send packet\n");
        // Add player to the list of players
        // player id, set that part of players to the correct data
        printf("Server: Creating player connection\n");
        playerConnection = malloc(sizeof(PlayerConnection));
        playerConnections[playerId] = playerConnection;
        playerConnection->socket = connectingSocket;
        playerConnection->playerId = numPlayers;
        printf("Server: Created player connection\n");

        printf("Server: Creating player object\n");
        // Init player data going into players list
        player = malloc(sizeof(Player));
        player->pos = (Vector2){0, 0};
        player->rot = 0.0f;
        player->vel = (Vector2){0, 0};
        player->isActive = 0b1;
        printf("Server: Created player object\n");
        players[playerId] = player;
        printf("Server: Attaching player object\n");

        printf("Server: Player %d Joined, lobby now %d/%d\n", numPlayers,
               numPlayers + 1, MAX_PLAYERS);
        numPlayers++;
        if (numPlayers == NUM_PLAYERS_TO_START) {
          break;
        }

      } else if (gameState->gameState == GAME_ONGOING) {
        printf("Server: Client attempting to join ongoing game\n");
        sendPacket.type = PACKET_GAME_STATUS;
        sendPacket.data.gameStatus.canJoin = 0b0;
        sendPacket.data.gameStatus.gameStarted = 0b1;
        printf("Cannot connect to ongoing game\n");
        continue;
      } else {
        sendPacket.type = PACKET_GAME_STATUS;
        sendPacket.data.gameStatus.canJoin = 0b0;
        printf("Cannot connect to game\n");
        continue;
      }
    } else {
      printf("Server: Error, packet is not of type GameRequest, instead of "
             "type: %d",
             recvPacket.type);
      continue;
    }
    // Connections are done, start the game

    // As player joins, add to list of players, each will need their own
    // worker thread
    /*pthread_t thread_id;*/
    /*pthread_create(&thread_id, NULL, handleClientConnection, clientData);*/
    /*pthread_join(thread_id, NULL);*/
  }

  serverStartGame();
  // done with connections, can close socket
  close(socketFd);
  return 0;
}

void *broadcastThreadFunction() { return NULL; }

void serverStartGame() {
  Color playerColors[] = {RED, YELLOW, GREEN, BLUE, PINK};
  // Send out the init packets
  PlayerConnection *pc;
  pc = playerConnections[0];
  while (pc != NULL) {
    // Send a GameInit to all of the PlayerConnections, takes the # of players,
    // their id, and the array of Colors
  }
  // Spin off broadcast thread to send out packets to clients
  pthread_t broadcastThreadId;
  pthread_create(&broadcastThreadId, NULL, broadcastThreadFunction(), NULL);
  pthread_join(broadcastThreadId, NULL);
}

int setupServer() {
  char host[] = "localhost";
  int port = 9876;
  // init the list of players;
  playerConnections = malloc(sizeof(PlayerConnection *) * MAX_PLAYERS);
  // memset to make sure they are null
  memset(playerConnections, 0, sizeof(PlayerConnection *) * MAX_PLAYERS);
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
