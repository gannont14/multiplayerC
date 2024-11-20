#include "raylib.h"
#include <stdint.h>
#include <sys/types.h>
#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#define MAX_PLAYERS 5

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define DEFAULT_PLAYER_SCALE 1
#define DEFUALT_PLAYER_THICKNESS 1
#define PLAYER_FORWARD_SPEED 15.0f
#define PLAYER_ROTATION_SPEED 3.0f
typedef struct {
  Vector2 pos;
  Vector2 vel;
  float rot;
  uint8_t isActive;
} Player;

typedef struct {
  int socket;
  int playerId;
} PlayerConnection;

enum ValidGameState {
  GAME_WAITING,
  GAME_ONGOING,
  GAME_ENDING,
};

typedef struct {
  enum ValidGameState gameState;
  int numPlayersConnected;
} GameState;

#endif
