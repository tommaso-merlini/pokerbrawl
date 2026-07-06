#include "game.h"

#include "../game_state.h"
#include "raylib.h"

#define SPAWNPOINT_SIZE 24.0f
#define PLAYER_SPEED 320.0f
#define PLAYER_JUMP_SPEED 700.0f
#define PLAYER_GRAVITY 1800.0f

typedef struct PlayerControls {
  int leftKey;
  int rightKey;
  int jumpKey;
} PlayerControls;

typedef struct PlayerColors {
  Color fill;
  Color outline;
} PlayerColors;

static const PlayerControls PLAYER_CONTROLS[MAX_PLAYERS] = {
    {KEY_A, KEY_D, KEY_W},
    {KEY_LEFT, KEY_RIGHT, KEY_UP},
    {KEY_J, KEY_L, KEY_I},
    {KEY_F, KEY_H, KEY_T},
};

static const PlayerColors PLAYER_COLORS[MAX_PLAYERS] = {
    {{35, 115, 230, 255}, {12, 45, 120, 255}},
    {{238, 118, 52, 255}, {128, 50, 18, 255}},
    {{45, 165, 95, 255}, {18, 86, 48, 255}},
    {{150, 95, 220, 255}, {75, 40, 135, 255}},
};

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

static void updateplayer(Player *player, const ArenaMap *map,
                         const PlayerControls *controls, float dt) {
  float move = 0.0f;

  if (IsKeyDown(controls->leftKey)) {
    move -= 1.0f;
  }
  if (IsKeyDown(controls->rightKey)) {
    move += 1.0f;
  }

  player->velocity.x = move * PLAYER_SPEED;

  if (IsKeyPressed(controls->jumpKey) && player->onGround) {
    player->velocity.y = -PLAYER_JUMP_SPEED;
    player->onGround = false;
  }

  player->position.x += player->velocity.x * dt;
  resolveplayerhorizontal(player, map);

  player->velocity.y += PLAYER_GRAVITY * dt;
  player->position.y += player->velocity.y * dt;
  resolveplayervertical(player, map);
}

static Vector2 maptoscreen(Vector2 point, const ArenaMap *map, int screenWidth,
                           int screenHeight) {
  float scaleX = (float)screenWidth / (float)map->width;
  float scaleY = (float)screenHeight / (float)map->height;
  float scale = scaleX < scaleY ? scaleX : scaleY;
  float offsetX = ((float)screenWidth - (float)map->width * scale) * 0.5f;
  float offsetY = ((float)screenHeight - (float)map->height * scale) * 0.5f;

  return (Vector2){offsetX + point.x * scale, offsetY + point.y * scale};
}

static void drawquad(const MapQuad *quad, const ArenaMap *map, int screenWidth,
                     int screenHeight) {
  Vector2 p0 = maptoscreen(quad->points[0], map, screenWidth, screenHeight);
  Vector2 p1 = maptoscreen(quad->points[1], map, screenWidth, screenHeight);
  Vector2 p2 = maptoscreen(quad->points[2], map, screenWidth, screenHeight);
  Vector2 p3 = maptoscreen(quad->points[3], map, screenWidth, screenHeight);
  Color fill = quad->collidable ? (Color){62, 140, 222, 230}
                                : (Color){190, 190, 190, 90};
  Color outline = quad->collidable ? (Color){20, 65, 130, 255}
                                   : (Color){105, 105, 105, 180};

  DrawTriangle(p0, p1, p2, fill);
  DrawTriangle(p0, p2, p3, fill);
  DrawLineEx(p0, p1, 3.0f, outline);
  DrawLineEx(p1, p2, 3.0f, outline);
  DrawLineEx(p2, p3, 3.0f, outline);
  DrawLineEx(p3, p0, 3.0f, outline);
}

static void drawspawnpoint(Vector2 spawnpoint, const ArenaMap *map,
                           int screenWidth, int screenHeight) {
  const float halfSize = SPAWNPOINT_SIZE * 0.5f;
  Vector2 topLeft = maptoscreen(
      (Vector2){spawnpoint.x - halfSize, spawnpoint.y - halfSize}, map,
      screenWidth, screenHeight);
  Vector2 bottomRight = maptoscreen(
      (Vector2){spawnpoint.x + halfSize, spawnpoint.y + halfSize}, map,
      screenWidth, screenHeight);
  Rectangle square = {topLeft.x, topLeft.y, bottomRight.x - topLeft.x,
                      bottomRight.y - topLeft.y};

  DrawRectangleRec(square, RED);
}

static void drawplayer(const Player *player, const ArenaMap *map,
                       const PlayerColors *colors, int screenWidth,
                       int screenHeight) {
  Rectangle bounds = rectfromcenter(player->position, player->size);
  Vector2 topLeft =
      maptoscreen((Vector2){bounds.x, bounds.y}, map, screenWidth,
                  screenHeight);
  Vector2 bottomRight =
      maptoscreen((Vector2){bounds.x + bounds.width, bounds.y + bounds.height},
                  map, screenWidth, screenHeight);
  Rectangle screenRect = {topLeft.x, topLeft.y, bottomRight.x - topLeft.x,
                          bottomRight.y - topLeft.y};

  DrawRectangleRec(screenRect, colors->fill);
  DrawRectangleLinesEx(screenRect, 2.0f, colors->outline);
}

void gameScreenUpdate(void) {
  if (!gGame.currentMap.loaded) {
    return;
  }

  float dt = GetFrameTime();

  for (int i = 0; i < gGame.playerCount && i < MAX_PLAYERS; i++) {
    if (gGame.players[i].spawned) {
      updateplayer(&gGame.players[i], &gGame.currentMap, &PLAYER_CONTROLS[i],
                   dt);
    }
  }
}

void gameScreenDraw(int currentWidth, int currentHeight) {
  ClearBackground((Color){28, 34, 42, 255});

  if (!gGame.currentMap.loaded) {
    DrawText(gGame.mapLoadError[0] != '\0' ? gGame.mapLoadError
                                           : "No map loaded",
             30, 30, 22, RED);
    return;
  }

  for (int i = 0; i < gGame.currentMap.quadCount; i++) {
    drawquad(&gGame.currentMap.quads[i], &gGame.currentMap, currentWidth,
             currentHeight);
  }

  for (int i = 0; i < gGame.currentMap.spawnpointCount; i++) {
    drawspawnpoint(gGame.currentMap.spawnpoints[i], &gGame.currentMap,
                   currentWidth, currentHeight);
  }

  for (int i = 0; i < gGame.playerCount && i < MAX_PLAYERS; i++) {
    if (gGame.players[i].spawned) {
      drawplayer(&gGame.players[i], &gGame.currentMap, &PLAYER_COLORS[i],
                 currentWidth, currentHeight);
    }
  }

  DrawText(gGame.currentMap.name, 24, 24, 24, RAYWHITE);
}
