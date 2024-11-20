#include "gameConfig.h"
#include "raylib.h"
#include <stdint.h>
#include <sys/_types/_u_int.h>

typedef struct {
  char playerName[32];
  Color colorPref;
} JoinRequest;

typedef struct {
  uint8_t canJoin;   // signals if player can join
  u_int gameStarted; // if can't join, but game ongoing, wait until not
} GameStatus;

typedef struct {
  uint8_t numPlayers;
  uint8_t playerId; // player's ID, use to find color
  Color playerColors[];
} GameInit;

typedef struct {
  uint8_t input;
  float rot;
} PlayerInput;

typedef struct {
  Player **players;
} PlayersData;

enum PacketType {
  // Before game
  PACKET_JOIN_REQUEST, // client -> server
  PACKET_GAME_STATUS,  // server -> client
  // Game about to start
  PACKET_GAME_INIT, // server -> client
  // Game ongoing
  PACKET_PLAYER_INPUT, // client -> server
  PACKET_PLAYERS_DATA, // server -> client
};

// Client: init
// Server: init response

// Client: wait for gameInit
// Server: gameInit

// Client: send playerInput packet
// Server: send PlayerPosition packet

// game over
typedef struct {
  enum PacketType type;
  union {
    JoinRequest joinRequest;
    GameStatus gameStatus;
    GameInit gameInit;
    PlayerInput playerInput;
    PlayersData playersData;
  } data;
} GamePacket;
