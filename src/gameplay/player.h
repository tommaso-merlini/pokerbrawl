#ifndef GAMEPLAY_PLAYER_H
#define GAMEPLAY_PLAYER_H

#include "../character.h"
#include "../game/constants.h"
#include "../input/input.h"
#include "../maps/map.h"

#include <stdbool.h>

typedef struct Player {
  Vector2 position;
  Vector2 velocity;
  Vector2 size;
  int healthPoints;
  float hurtTimer;
  Character character;
  bool facingLeft;
  bool onGround;
  bool spawned;
} Player;

void initPlayer(Player *player, Vector2 spawnpoint, Character character);
void updatePlayer(Player *player, const ArenaMap *map, PlayerInput input,
                  float dt);

#endif
