#include "player.h"

#include "collision.h"

#define PLAYER_SPEED 500.0f
#define PLAYER_JUMP_SPEED 900.0f
#define PLAYER_GRAVITY 1800.0f
#define PLAYER_HURT_CONTROL_MULTIPLIER 0.15f
#define PLAYER_DASH_SPEED 1100.0f
#define PLAYER_DASH_DURATION 0.16f
#define PLAYER_DASH_COOLDOWN 0.55f

static float decreaseTimer(float timer, float dt) {
  timer -= dt;
  return timer > 0.0f ? timer : 0.0f;
}

static void startDash(Player *player, float move) {
  if (move < 0.0f) {
    player->dashDirection = -1.0f;
  } else if (move > 0.0f) {
    player->dashDirection = 1.0f;
  } else {
    player->dashDirection = player->facingLeft ? -1.0f : 1.0f;
  }

  player->facingLeft = player->dashDirection < 0.0f;
  player->dashTimer = PLAYER_DASH_DURATION;
  player->dashCooldownTimer = PLAYER_DASH_COOLDOWN;
}

void initPlayer(Player *player, Vector2 spawnpoint, Character character) {
  *player = (Player){
      .character = character,
      .lives = INITIAL_PLAYER_LIVES,
      .jumps = 0,
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
  player->dashCooldownTimer = decreaseTimer(player->dashCooldownTimer, dt);

  if (player->hurtTimer > 0.0f) {
    player->hurtTimer = decreaseTimer(player->hurtTimer, dt);
    player->dashTimer = 0.0f;

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

    if (input.dashPressed && player->dashCooldownTimer <= 0.0f) {
      startDash(player, input.move);
    }

    if (player->dashTimer > 0.0f) {
      player->velocity.x = player->dashDirection * PLAYER_DASH_SPEED;
      player->dashTimer = decreaseTimer(player->dashTimer, dt);
    } else {
      player->velocity.x = input.move * PLAYER_SPEED;

      if (input.jumpPressed && player->jumps < 2) {
        player->velocity.y = -PLAYER_JUMP_SPEED;
        player->jumps = player->jumps + 1;
      }
    }
  }

  player->position.x += player->velocity.x * dt;
  resolvePlayerHorizontal(player, map);

  player->velocity.y += PLAYER_GRAVITY * dt;
  player->position.y += player->velocity.y * dt;
  resolvePlayerVertical(player, map);
}
