#include "player.h"

#include "collision.h"

#include <stddef.h>

#define PLAYER_SPEED 500.0f
#define PLAYER_JUMP_SPEED 1000.0f
#define PLAYER_GRAVITY 1800.0f
#define PLAYER_HURT_CONTROL_MULTIPLIER 0.15f
#define PLAYER_DASH_SPEED 1100.0f
#define PLAYER_DASH_DURATION 0.16f
#define PLAYER_DASH_COOLDOWN 0.55f
#define PLAYER_DROP_DURATION 0.25f
#define PLAYER_SEPARATION_SPEED 240.0f

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

// Invisible walls at the arena edges. They stop walking and dashing, but a
// player sent flying by a hit can still be knocked out of the arena (otherwise
// with a full-width floor nobody could ever lose a life).
static void keepInsideArena(Player *player, const ArenaMap *map,
                            float previousX) {
  if (player->hurtTimer > 0.0f) {
    return;
  }

  float minX = player->size.x * 0.5f;
  float maxX = (float)map->width - player->size.x * 0.5f;
  bool wasInside = previousX >= minX && previousX <= maxX;

  if (wasInside) {
    if (player->position.x < minX) {
      player->position.x = minX;
    } else if (player->position.x > maxX) {
      player->position.x = maxX;
    } else {
      return;
    }
  } else if ((previousX < minX && player->position.x < previousX) ||
             (previousX > maxX && player->position.x > previousX)) {
    // Already knocked out: can move back in, but not further out.
    player->position.x = previousX;
  } else {
    return;
  }

  player->velocity.x = 0.0f;
  player->dashTimer = 0.0f;
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
      .lives = lives,
      .character = character,
      .spawned = true,
  };
}

void updatePlayer(Player *player, const ArenaMap *map, const PlayerList *others,
                  PlayerInput input, float dt) {
  player->dashCooldownTimer = decreaseTimer(player->dashCooldownTimer, dt);
  player->dropTimer = decreaseTimer(player->dropTimer, dt);

  if (input.dropPressed && player->onPlatform) {
    player->dropTimer = PLAYER_DROP_DURATION;
    player->onPlatform = false;
    player->onGround = false;
  }

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

  PlayerList solidPlayers = {0};
  float push = 0.0f;
  if (others != NULL) {
    solidPlayers = *others;
    solidPlayers.ignoreMask = overlappingPlayersMask(player, others);
    push = separationPush(player, &solidPlayers, solidPlayers.ignoreMask,
                          PLAYER_SEPARATION_SPEED * dt);
  }

  float previousX = player->position.x;
  float moveX = player->velocity.x * dt + push;
  player->position.x += moveX;
  resolvePlayerHorizontal(player, map, &solidPlayers, moveX);
  keepInsideArena(player, map, previousX);

  float previousBottom = player->position.y + player->size.y * 0.5f;
  player->velocity.y += PLAYER_GRAVITY * dt;
  player->position.y += player->velocity.y * dt;
  resolvePlayerVertical(player, map, &solidPlayers, previousBottom);
}
