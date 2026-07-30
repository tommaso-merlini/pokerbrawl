#include "hit.h"

#include "../game/game_state.h"
#include <stdio.h>

#define PLAYER_HURT_DURATION 0.28f
#define BODY_HIT_KNOCKBACK_INCREASE 10
#define BODY_HIT_OFFSET 60.0f
#define BODY_KNOCKBACK_HORIZONTAL 620.0f
#define BODY_KNOCKBACK_VERTICAL 420.0f

static Rectangle playerBounds(const Player *player) {
  return (Rectangle){
      player->position.x - player->size.x * 0.5f,
      player->position.y - player->size.y * 0.5f,
      player->size.x,
      player->size.y,
  };
}

static bool hitPlayer(GameState *game, const Hit *hit) {
  for (int i = 0; i < game->playerCount && i < MAX_PLAYERS; i++) {
    Player *player = &game->players[i];
    if (i == hit->attackerIndex || !player->spawned) {
      continue;
    }

    if (!CheckCollisionPointRec(hit->position, playerBounds(player))) {
      continue;
    }

    player->knockbackPercent += hit->knockbackIncrease;
    float knockbackMultiplier = 1.0f + player->knockbackPercent / 100.0f;
    player->velocity = (Vector2){
        hit->knockback.x * knockbackMultiplier,
        hit->knockback.y * knockbackMultiplier,
    };
    player->hurtTimer = PLAYER_HURT_DURATION;
    player->dashTimer = 0.0f;
    player->onGround = false;
    return true;
  }

  return false;
}

static void removeHit(GameState *game, int index) {
  game->hitCount--;
  game->hits[index] = game->hits[game->hitCount];
  game->hits[game->hitCount] = (Hit){0};
}

bool addHit(GameState *game, Hit hit) {
  if (!game || game->hitCount >= MAX_HITS) {
    return false;
  }

  game->hits[game->hitCount++] = hit;
  return true;
}

void createBodyHit(GameState *game, int attackerIndex) {
  const Player *attacker = &game->players[attackerIndex];
  float direction = attacker->facingLeft ? -1.0f : 1.0f;

  addHit(game, (Hit){
                   .attackerIndex = attackerIndex,
                   .type = BODY,
                   .knockbackIncrease = BODY_HIT_KNOCKBACK_INCREASE,
                   .position = {
                       attacker->position.x + direction * BODY_HIT_OFFSET,
                       attacker->position.y,
                   },
                   .knockback = {
                       direction * BODY_KNOCKBACK_HORIZONTAL,
                       -BODY_KNOCKBACK_VERTICAL,
                   },
               });
}

void clearHits(GameState *game) {
  if (!game) {
    return;
  }

  while (game->hitCount > 0) {
    game->hits[--game->hitCount] = (Hit){0};
  }
}

void updateHits(GameState *game, float dt) {
  if (!game) {
    return;
  }

  int i = 0;
  while (i < game->hitCount) {
    Hit *hit = &game->hits[i];

    if (hit->type == BODY) {
      if (hit->resolved) {
        removeHit(game, i);
        continue;
      }

      hitPlayer(game, hit);
      hit->resolved = true;
      i++;
      continue;
    }

    if (hit->type == PROJECTILE) {
      hit->position.x += hit->velocity.x * dt;
      hit->position.y += hit->velocity.y * dt;
      hit->lifetime -= dt;
    }

    bool finished = hitPlayer(game, hit) || hit->lifetime <= 0.0f;
    if (finished) {
      removeHit(game, i);
    } else {
      i++;
    }
  }
}
