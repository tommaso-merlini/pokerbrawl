#include "hit.h"
#include <stdio.h>

#include "../game/game_state.h"

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

    player->healthPoints -= hit->damage;
    printf("player hit!\n");
    if (player->healthPoints < 0) {
      player->healthPoints = 0;
    }
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
  printf("add hit!\n");
  if (!game || game->hitCount >= MAX_HITS) {
    return false;
  }

  game->hits[game->hitCount++] = hit;
  return true;
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

    if (hit->type == PROJECTILE) {
      hit->position.x += hit->velocity.x * dt;
      hit->position.y += hit->velocity.y * dt;
      hit->lifetime -= dt;
    }

    bool finished = hitPlayer(game, hit) || hit->type == BODY ||
                    hit->lifetime <= 0.0f;
    if (finished) {
      removeHit(game, i);
    } else {
      i++;
    }
  }
}
