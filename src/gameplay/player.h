#ifndef GAMEPLAY_PLAYER_H
#define GAMEPLAY_PLAYER_H

#include "../character.h"
#include "../game/constants.h"
#include "../input/keyboard.h"
#include "../maps/map.h"
#include "collision.h"

#include <stdbool.h>

typedef struct Player {
  Vector2 position;
  Vector2 velocity;
  Vector2 size;
  int knockbackPercent;
  int lives;
  float hurtTimer;
  float dashTimer;
  float dashCooldownTimer;
  float dashDirection;
  float dropTimer;
  Character character;
  bool facingLeft;
  bool onGround;
  bool onPlatform;
  bool spawned;
  int jumps;
} Player;

void initPlayer(Player *player, Vector2 spawnpoint, Character character);
void respawnPlayer(Player *player, Vector2 spawnpoint);
void updatePlayer(Player *player, const ArenaMap *map, const PlayerList *others,
                  PlayerInput input, float dt);

#endif
