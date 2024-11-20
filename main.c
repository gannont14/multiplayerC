#include "client.h"
#include "gameConfig.h"
#include "raylib.h"
#include "raymath.h"
#include "server.h"
#include "transport.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

/*const int WINDOW_WIDTH = 800;*/
/*const int WINDOW_HEIGHT = 800;*/
/*const int DEFAULT_PLAYER_SCALE = 1;*/
/*const int DEFUALT_PLAYER_THICKNESS = 1;*/
/*const float PLAYER_FORWARD_SPEED = 15.0f;*/
/*const float PLAYER_ROTATION_SPEED = 3.0f;*/

/*const float deltaTime = 0.016f;*/

void drawShape(Vector2 startPos, Vector2 *vectors, int vectorsLen, float rot,
               float scale, Color color) {

  float rotationInRadians = rot * (M_PI / 180);

  Color defaultColor = RED;

  for (int i = 0; i < vectorsLen; i++) {
    // Draw line from the vector the vector + 1, wrapping with % vectorlen
    DrawLineEx(
        Vector2Add(
            startPos,
            Vector2Scale(Vector2Rotate(vectors[i], rotationInRadians), scale)),
        Vector2Add(startPos,
                   Vector2Scale(Vector2Rotate(vectors[(i + 1) % vectorsLen],
                                              rotationInRadians),
                                scale)),
        DEFUALT_PLAYER_THICKNESS, color);
  }
}

void drawPlayer(Vector2 startPos, float rot, float scale, Color color) {
  // default player shape, could chage to whatever, right now just square
  int playerShapeLen = 4;
  Vector2 playerShape[4] =
      (Vector2[]){{10, 10}, {10, -10}, {-10, -10}, {-10, 10}};
  int extrusionShapeLen = 2;
  Vector2 extrusionShape[2] = (Vector2[]){{0, 0}, {20, 0}};

  int rotationInRadians = rot * (M_PI / 180);
  // Draw player outline (square)
  drawShape(startPos, playerShape, playerShapeLen, rot, scale, color);
  // Draw extrusion for thing
  drawShape(startPos, extrusionShape, extrusionShapeLen, rot, scale, color);
}

void updatePlayer(Player *player) {
  float deltaTime = GetFrameTime();

  float rotationInRadians = player->rot * (M_PI / 180.0f);
  // Add rotaiton
  if (IsKeyDown(KEY_RIGHT))
    player->rot += PLAYER_ROTATION_SPEED;
  if (IsKeyDown(KEY_LEFT))
    player->rot -= PLAYER_ROTATION_SPEED;

  Vector2 direction = (Vector2){
      cos(rotationInRadians),
      sin(rotationInRadians),
  };

  // can't % between float and int, + 360 so it's positive
  player->rot = fmodf(player->rot + 360.0f, 360.0f);

  // check for key presses
  // Add forward velocity
  if (IsKeyDown(KEY_UP)) {
    player->vel.x += direction.x * PLAYER_FORWARD_SPEED;
    player->vel.y += direction.y * PLAYER_FORWARD_SPEED;
  }
  if (IsKeyDown(KEY_DOWN)) {
    player->vel.x -= direction.x * PLAYER_FORWARD_SPEED;
    player->vel.y -= direction.y * PLAYER_FORWARD_SPEED;
  }

  // add velocity

  player->pos.x += player->vel.x * deltaTime;
  player->pos.y += player->vel.y * deltaTime;

  drawPlayer(player->pos, player->rot, DEFAULT_PLAYER_SCALE, RED);

  // dampening velocity
  player->vel.x *= 0.8f;
  player->vel.y *= 0.8f;
}

int main(int argc, char *argv[]) {
  // Check args for client or server call
  if (argc != 2) {
    printf("Specify [client | server]\n");
    return 1;
  }

  if (strcmp(argv[1], "server") == 0) {
    setupServer();
    return 1;
  }
  if (strcmp(argv[1], "test") == 0) {
    int numClients = 2;
    for (int i = 0; i < numClients; i++) {
      if (fork() == 0) {
        // child process
        printf("Client: calling setupClient>%d\n", i);
        setupClient();
        return 0;
      }
    }
    // parent process waiting
    for (int i = 0; i < numClients; i++) {
      wait(NULL);
    }
    return 1;
  }

  if (strcmp(argv[1], "client") != 0) {
    printf("Invalid option\n");
    return 1;
  }

  int socketFd = setupClient();
  // Connection created with the
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
             "raylib [core] example - basic window");
  SetTargetFPS(60);

  Player player = {(Vector2){GetScreenWidth() / 2, GetScreenHeight() / 2},
                   (Vector2){0, 0}, 0, 1};

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    /*DrawLineEx((Vector2){0, 0}, (Vector2){500, 500}, 3.0, RED);*/

    // draw line using new function
    /*drawShape((Vector2){GetScreenWidth() / 2, GetScreenHeight() / 2},*/
    /*          (Vector2[]){{10, 10}, {10, -10}, {-10, -10}, {-10, 10}}, 4,
     * 45,*/
    /*          2);*/

    /*updatePlayer(&player);*/
    sendPlayerInput(socketFd);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
