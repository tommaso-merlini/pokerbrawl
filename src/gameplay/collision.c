#include "collision.h"

#include "player.h"
#include "raylib.h"

static Rectangle rectangleFromCenter(Vector2 center, Vector2 size) {
  return (Rectangle){center.x - size.x * 0.5f, center.y - size.y * 0.5f, size.x,
                     size.y};
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

static bool collides(Rectangle player, const MapQuad *quad) {
  return quad->collidable && CheckCollisionRecs(player, quadBounds(quad));
}

void resolvePlayerHorizontal(Player *player, const ArenaMap *map) {
  Rectangle playerRect = rectangleFromCenter(player->position, player->size);

  for (int i = 0; i < map->quadCount; i++) {
    Rectangle solid = quadBounds(&map->quads[i]);
    if (!collides(playerRect, &map->quads[i])) {
      continue;
    }

    if (player->velocity.x > 0.0f) {
      player->position.x = solid.x - player->size.x * 0.5f;
    } else if (player->velocity.x < 0.0f) {
      player->position.x = solid.x + solid.width + player->size.x * 0.5f;
    }

    player->velocity.x = 0.0f;
    playerRect = rectangleFromCenter(player->position, player->size);
  }
}

void resolvePlayerVertical(Player *player, const ArenaMap *map) {
  Rectangle playerRect = rectangleFromCenter(player->position, player->size);
  player->onGround = false;

  for (int i = 0; i < map->quadCount; i++) {
    Rectangle solid = quadBounds(&map->quads[i]);
    if (!collides(playerRect, &map->quads[i])) {
      continue;
    }

    if (player->velocity.y > 0.0f) {
      player->position.y = solid.y - player->size.y * 0.5f;
      player->onGround = true;
    } else if (player->velocity.y < 0.0f) {
      player->position.y = solid.y + solid.height + player->size.y * 0.5f;
    }

    player->velocity.y = 0.0f;
    playerRect = rectangleFromCenter(player->position, player->size);
  }
}
