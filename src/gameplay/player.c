#include "player.h"

#include "collision.h"

#define PLAYER_WIDTH 36.0f
#define PLAYER_HEIGHT 54.0f
#define PLAYER_SPEED 320.0f
#define PLAYER_JUMP_SPEED 700.0f
#define PLAYER_GRAVITY 1800.0f

void initPlayer(Player *player, Vector2 spawnpoint, Character character) {
  *player = (Player){
      .position = spawnpoint,
      .size = {PLAYER_WIDTH, PLAYER_HEIGHT},
      .character = character,
      .spawned = true,
  };
}

void updatePlayer(Player *player, const ArenaMap *map, PlayerInput input,
                  float dt) {
  player->velocity.x = input.move * PLAYER_SPEED;

  if (input.jumpPressed && player->onGround) {
    player->velocity.y = -PLAYER_JUMP_SPEED;
    player->onGround = false;
  }

  player->position.x += player->velocity.x * dt;
  resolvePlayerHorizontal(player, map);

  player->velocity.y += PLAYER_GRAVITY * dt;
  player->position.y += player->velocity.y * dt;
  resolvePlayerVertical(player, map);
}
