#ifndef HIT_H
#define HIT_H

#include "raylib.h"

#include <stdbool.h>

#define MAX_HITS 64

typedef enum HitType { BODY, PROJECTILE } HitType;

typedef struct Hit {
  int attackerIndex;
  HitType type;
  int damage;
  Vector2 position;
  Vector2 velocity;
  float lifetime;
} Hit;

struct GameState;

bool addHit(struct GameState *game, Hit hit);
void clearHits(struct GameState *game);
void updateHits(struct GameState *game, float dt);

#endif
