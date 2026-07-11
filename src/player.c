#include "player.h"

#include "raylib.h"

#define PLAYER_WIDTH 36.0f
#define PLAYER_HEIGHT 54.0f
#define PLAYER_SPEED 320.0f
#define PLAYER_JUMP_SPEED 700.0f
#define PLAYER_GRAVITY 1800.0f

static Rectangle rectfromcenter(Vector2 center, Vector2 size) {
  return (Rectangle){center.x - size.x * 0.5f, center.y - size.y * 0.5f,
                     size.x, size.y};
}

static Rectangle quadbounds(const MapQuad *quad) {
  float minX = quad->points[0].x;
  float maxX = quad->points[0].x;
  float minY = quad->points[0].y;
  float maxY = quad->points[0].y;

  for (int i = 1; i < 4; i++) {
    if (quad->points[i].x < minX) {
      minX = quad->points[i].x;
    }
    if (quad->points[i].x > maxX) {
      maxX = quad->points[i].x;
    }
    if (quad->points[i].y < minY) {
      minY = quad->points[i].y;
    }
    if (quad->points[i].y > maxY) {
      maxY = quad->points[i].y;
    }
  }

  return (Rectangle){minX, minY, maxX - minX, maxY - minY};
}

static void resolveplayerhorizontal(Player *player, const ArenaMap *map) {
  Rectangle playerRect = rectfromcenter(player->position, player->size);

  for (int i = 0; i < map->quadCount; i++) {
    if (!map->quads[i].collidable) {
      continue;
    }

    Rectangle solid = quadbounds(&map->quads[i]);

    if (!CheckCollisionRecs(playerRect, solid)) {
      continue;
    }

    if (player->velocity.x > 0.0f) {
      player->position.x = solid.x - player->size.x * 0.5f;
    } else if (player->velocity.x < 0.0f) {
      player->position.x = solid.x + solid.width + player->size.x * 0.5f;
    }

    player->velocity.x = 0.0f;
    playerRect = rectfromcenter(player->position, player->size);
  }
}

static void resolveplayervertical(Player *player, const ArenaMap *map) {
  Rectangle playerRect = rectfromcenter(player->position, player->size);
  player->onGround = false;

  for (int i = 0; i < map->quadCount; i++) {
    if (!map->quads[i].collidable) {
      continue;
    }

    Rectangle solid = quadbounds(&map->quads[i]);

    if (!CheckCollisionRecs(playerRect, solid)) {
      continue;
    }

    if (player->velocity.y > 0.0f) {
      player->position.y = solid.y - player->size.y * 0.5f;
      player->onGround = true;
    } else if (player->velocity.y < 0.0f) {
      player->position.y = solid.y + solid.height + player->size.y * 0.5f;
    }

    player->velocity.y = 0.0f;
    playerRect = rectfromcenter(player->position, player->size);
  }
}

void initPlayer(Player *player, Vector2 spawnpoint, Character character) {
  *player = (Player){0};
  player->position = spawnpoint;
  player->velocity = (Vector2){0.0f, 0.0f};
  player->size = (Vector2){PLAYER_WIDTH, PLAYER_HEIGHT};
  player->character = character;
  player->onGround = false;
  player->spawned = true;
}

void updatePlayer(Player *player, const ArenaMap *map, PlayerInput input,
                  float dt) {
  player->velocity.x = input.move * PLAYER_SPEED;

  if (input.jumpPressed && player->onGround) {
    player->velocity.y = -PLAYER_JUMP_SPEED;
    player->onGround = false;
  }

  player->position.x += player->velocity.x * dt;
  resolveplayerhorizontal(player, map);

  player->velocity.y += PLAYER_GRAVITY * dt;
  player->position.y += player->velocity.y * dt;
  resolveplayervertical(player, map);
}
