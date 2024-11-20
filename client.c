#include "transport.h"
#include <ctype.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int clientGenerateSocket(const char *host, int port) {
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

  return socketFd;
}

void sendPlayerInput(int socketFd) {

  PlayerInput input;
  uint8_t preInput = input.input;
  preInput = 0;
  if (IsKeyDown(KEY_UP))
    preInput += 0b01000;
  if (IsKeyDown(KEY_DOWN))
    preInput += 0b00100;
  if (IsKeyDown(KEY_LEFT))
    preInput += 0b00010;
  if (IsKeyDown(KEY_RIGHT))
    preInput += 0b00001;
  input.input = preInput;

  printf("Client Input: %d, %d\n", input.input, preInput);

  send(socketFd, (const void *)&input, sizeof(input), 0);
}

int clientSendJoinRequest(int socket, char *playerName, Color colorPref) {

  // request to see if the server is ready for us to join
  GamePacket packet;
  packet.type = PACKET_JOIN_REQUEST;
  strncpy(packet.data.joinRequest.playerName, playerName, 31);
  // ensure that it is null terminated
  packet.data.joinRequest.playerName[31] = '\0';
  packet.data.joinRequest.colorPref = colorPref;
  // send the packet
  int bytesSent = send(socket, &packet, sizeof(packet), 0);
  return bytesSent;
}

int clientJoinGame(int socket) {
  // Client has joined the game, begin listening for the updates from the
  // server.
  // Enter the main loop, will be receiving the init from the server,
  // after init is receiving, parse information, and then recv playerdata
  GameInit recvdGameInit;
  if (recv(socket, &recvdGameInit, sizeof(recvdGameInit), 0) < 0) {
    printf("error receiving game init data from server\n");
    return -1;
  }
  // data has been received, update current players id, player colors, numPlayer
  int numPlayers = recvdGameInit.numPlayers;
  int playerId = recvdGameInit.playerId;
  Color *playerColors = recvdGameInit.playerColors;

  printf("Client successfully received initialization data, waiting for game "
         "start\n");

  // now client should start receiving player objects from the server
  Player **players;
  // Enter main loop
  int connected = 1;
  // init the raylib window
  printf("Client: player connected\n");
  /*InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT,*/
  /*           "raylib [core] example - basic window");*/
  /*SetTargetFPS(60);*/
  /**/
  /*while (!WindowShouldClose()) {*/
  /*  BeginDrawing();*/
  /*  ClearBackground(BLACK);*/
  /**/
  /*  // Send the player input,*/
  /*  sendPlayerInput(socket);*/
  /*  // Recv players from server*/
  /*  recv(socket, &players, sizeof(players) * numPlayers, 0);*/
  /*  // Render to screen*/
  /*  // TODO - renderPlayers*/
  /*  EndDrawing();*/
  /*}*/
  /**/
  /*CloseWindow();*/

  return 0;
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

  int clientSocket = clientGenerateSocket(host, port);

  if (clientSocket > 0) {
    printf("Client socket successfully created\n");
  }
  int bytesSent;

  bytesSent = clientSendJoinRequest(clientSocket, "TestPlayer", RED);
  if (bytesSent < 0) {
    printf("Client: Error sending join request\n");
  }

  // create a packet to recv from the server, will be of type GameStatus
  // Either, canJoin, or GameFull
  GamePacket recvdPacket;
  if (recv(clientSocket, &recvdPacket, sizeof(recvdPacket), 0) < 0) {
    printf("Error receiving status from server\n");
    return -1;
  }

  if (recvdPacket.type == PACKET_GAME_STATUS) {
    // check if game is full or not
    if (recvdPacket.data.gameStatus.canJoin == 0b1) {
      // Player can join the game, go through logic
      clientJoinGame(clientSocket);
    } else {
      // Player cannot join the game, return for now, will put in loop to
      // retry...
      return -1;
    }
  } else {
    printf("Error within packets, returning...\n");
    return -1;
  }

  return 0;
}
