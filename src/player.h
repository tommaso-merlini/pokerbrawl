#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#include "character.h"
#include "map.h"

#define MAX_PLAYERS 4
#define DEFAULT_PLAYER_COUNT 2

typedef struct Player {
  Vector2 position;
  Vector2 velocity;
  Vector2 size;
  Character character;
  bool onGround;
  bool spawned;
} Player;

typedef struct PlayerInput {
  float move;
  bool jumpPressed;
} PlayerInput;

void initPlayer(Player *player, Vector2 spawnpoint, Character character);
void updatePlayer(Player *player, const ArenaMap *map, PlayerInput input,
                  float dt);

#endif
