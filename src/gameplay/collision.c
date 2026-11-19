#include "collision.h"

#include "player.h"
#include "raylib.h"

#include <stddef.h>

static Rectangle rectangleFromCenter(Vector2 center, Vector2 size) {
  return (Rectangle){center.x - size.x * 0.5f, center.y - size.y * 0.5f, size.x,
                     size.y};
}

static Rectangle playerRectangle(const Player *player) {
  return rectangleFromCenter(player->position, player->size);
}

static Rectangle quadBounds(const MapQuad *quad) {
  float minX = quad->points[0].x;
  float maxX = minX;
  float minY = quad->points[0].y;
  float maxY = minY;

  for (int i = 1; i < 4; i++) {
    minX = quad->points[i].x < minX ? quad->points[i].x : minX;
    maxX = quad->points[i].x > maxX ? quad->points[i].x : maxX;
    minY = quad->points[i].y < minY ? quad->points[i].y : minY;
    maxY = quad->points[i].y > maxY ? quad->points[i].y : maxY;
  }

  return (Rectangle){minX, minY, maxX - minX, maxY - minY};
}

// Solid quads block from every side. Platforms are handled separately: they
// only stop a player falling onto them from above.
static bool collidesSolid(Rectangle player, const MapQuad *quad) {
  return quad->collidable && !quad->platform &&
         CheckCollisionRecs(player, quadBounds(quad));
}

// Players that were already overlapping before this move (e.g. after a
// respawn on the same spawnpoint) are skipped, so they can walk apart instead
// of snapping through each other.
static bool isOtherSolidPlayer(const Player *player, const PlayerList *others,
                               int index) {
  const Player *other = &others->players[index];
  return other != player && other->spawned &&
         (others->ignoreMask & (1u << index)) == 0;
}

void resolvePlayerHorizontal(Player *player, const ArenaMap *map,
                             const PlayerList *others, float moveX) {
  Rectangle playerRect = playerRectangle(player);

  for (int i = 0; i < map->quadCount; i++) {
    Rectangle solid = quadBounds(&map->quads[i]);
    if (!collidesSolid(playerRect, &map->quads[i])) {
      continue;
    }

    if (moveX > 0.0f) {
      player->position.x = solid.x - player->size.x * 0.5f;
    } else if (moveX < 0.0f) {
      player->position.x = solid.x + solid.width + player->size.x * 0.5f;
    }

    player->velocity.x = 0.0f;
    player->dashTimer = 0.0f;
    playerRect = playerRectangle(player);
  }

  if (others == NULL) {
    return;
  }

  for (int i = 0; i < others->count; i++) {
    const Player *other = &others->players[i];
    if (!isOtherSolidPlayer(player, others, i)) {
      continue;
    }

    Rectangle otherRect = playerRectangle(other);
    if (!CheckCollisionRecs(playerRect, otherRect)) {
      continue;
    }

    if (moveX > 0.0f) {
      player->position.x = otherRect.x - player->size.x * 0.5f;
    } else if (moveX < 0.0f) {
      player->position.x =
          otherRect.x + otherRect.width + player->size.x * 0.5f;
    }

    // Knockback keeps its momentum so a hit player can't be "caught" forever.
    if (player->hurtTimer <= 0.0f) {
      player->velocity.x = 0.0f;
    }
    player->dashTimer = 0.0f;
    playerRect = playerRectangle(player);
  }
}

static void landOn(Player *player, float top) {
  player->position.y = top - player->size.y * 0.5f;
  player->velocity.y = 0.0f;
  player->onGround = true;
  player->jumps = 0;
}

void resolvePlayerVertical(Player *player, const ArenaMap *map,
                           const PlayerList *others, float previousBottom) {
  Rectangle playerRect = playerRectangle(player);
  float bottom = playerRect.y + playerRect.height;
  player->onGround = false;
  player->onPlatform = false;

  for (int i = 0; i < map->quadCount; i++) {
    const MapQuad *quad = &map->quads[i];
    Rectangle solid = quadBounds(quad);

    if (quad->collidable && quad->platform) {
      bool falling = player->velocity.y >= 0.0f;
      bool wasAbove = previousBottom <= solid.y + 0.5f;
      bool reachesTop = bottom >= solid.y;
      bool overlapsX = playerRect.x < solid.x + solid.width &&
                       playerRect.x + playerRect.width > solid.x;
      if (player->dropTimer <= 0.0f && falling && wasAbove && reachesTop &&
          overlapsX) {
        landOn(player, solid.y);
        player->onPlatform = true;
        playerRect = playerRectangle(player);
        bottom = playerRect.y + playerRect.height;
      }
      continue;
    }

    if (!collidesSolid(playerRect, quad)) {
      continue;
    }

    if (player->velocity.y > 0.0f) {
      landOn(player, solid.y);
    } else if (player->velocity.y < 0.0f) {
      player->position.y = solid.y + solid.height + player->size.y * 0.5f;
    }

    player->velocity.y = 0.0f;
    playerRect = playerRectangle(player);
    bottom = playerRect.y + playerRect.height;
  }

  if (others == NULL) {
    return;
  }

  for (int i = 0; i < others->count; i++) {
    const Player *other = &others->players[i];
    if (!isOtherSolidPlayer(player, others, i)) {
      continue;
    }

    Rectangle otherRect = playerRectangle(other);
    if (!CheckCollisionRecs(playerRect, otherRect)) {
      continue;
    }

    if (player->velocity.y > 0.0f) {
      // Standing on another player's head.
      landOn(player, otherRect.y);
    } else if (player->velocity.y < 0.0f) {
      player->position.y =
          otherRect.y + otherRect.height + player->size.y * 0.5f;
      player->velocity.y = 0.0f;
    }

    playerRect = playerRectangle(player);
  }
}

unsigned overlappingPlayersMask(const Player *player,
                                const PlayerList *others) {
  unsigned mask = 0;
  if (others == NULL) {
    return mask;
  }

  Rectangle playerRect = playerRectangle(player);
  for (int i = 0; i < others->count && i < 32; i++) {
    const Player *other = &others->players[i];
    if (other != player && other->spawned &&
        CheckCollisionRecs(playerRect, playerRectangle(other))) {
      mask |= 1u << i;
    }
  }
  return mask;
}

float separationPush(const Player *player, const PlayerList *others,
                     unsigned overlapMask, float maxPush) {
  float push = 0.0f;
  for (int i = 0; i < others->count && i < 32; i++) {
    if ((overlapMask & (1u << i)) == 0) {
      continue;
    }
    float dx = player->position.x - others->players[i].position.x;
    if (dx == 0.0f) {
      // Perfectly stacked: split deterministically by index.
      dx = player < &others->players[i] ? -1.0f : 1.0f;
    }
    push += dx < 0.0f ? -maxPush : maxPush;
  }
  return push;
}
