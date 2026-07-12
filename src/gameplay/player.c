#include "player.h"

#include "collision.h"

#define PLAYER_SPEED 400.0f
#define PLAYER_JUMP_SPEED 700.0f
#define PLAYER_GRAVITY 1800.0f
#define PLAYER_HURT_CONTROL_MULTIPLIER 0.15f

void initPlayer(Player *player, Vector2 spawnpoint, Character character) {
  *player = (Player){
      .character = character,
      .lives = INITIAL_PLAYER_LIVES,
  };
  respawnPlayer(player, spawnpoint);
}

void respawnPlayer(Player *player, Vector2 spawnpoint) {
  Character character = player->character;
  int lives = player->lives;

  *player = (Player){
      .position = spawnpoint,
      .size = {PLAYER_COLLIDER_WIDTH, PLAYER_COLLIDER_HEIGHT},
      .healthPoints = PLAYER_MAX_HEALTH,
      .lives = lives,
      .character = character,
      .spawned = true,
  };
}

void updatePlayer(Player *player, const ArenaMap *map, PlayerInput input,
                  float dt) {
  if (player->hurtTimer > 0.0f) {
    player->hurtTimer -= dt;
    if (player->hurtTimer < 0.0f) {
      player->hurtTimer = 0.0f;
    }

    // Keep most of the impact momentum while still allowing a little air
    // control. This prevents knockback from being cancelled on the next frame.
    player->velocity.x += input.move * PLAYER_SPEED *
                          PLAYER_HURT_CONTROL_MULTIPLIER * dt;
  } else {
    if (input.move < 0.0f) {
      player->facingLeft = true;
    } else if (input.move > 0.0f) {
      player->facingLeft = false;
    }

    player->velocity.x = input.move * PLAYER_SPEED;

    if (input.jumpPressed && player->onGround) {
      player->velocity.y = -PLAYER_JUMP_SPEED;
      player->onGround = false;
    }
  }

  player->position.x += player->velocity.x * dt;
  resolvePlayerHorizontal(player, map);

  player->velocity.y += PLAYER_GRAVITY * dt;
  player->position.y += player->velocity.y * dt;
  resolvePlayerVertical(player, map);
}
