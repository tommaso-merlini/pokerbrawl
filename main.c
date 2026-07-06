#include "raylib.h"
#include "map_loader.h"

#include <stdio.h>

#define MAP_FILE_PATH "mappe/mappe.json"
#define SPAWNPOINT_SIZE 24.0f
#define PLAYER_WIDTH 36.0f
#define PLAYER_HEIGHT 54.0f
#define PLAYER_SPEED 320.0f
#define PLAYER_JUMP_SPEED 700.0f
#define PLAYER_GRAVITY 1800.0f

typedef struct Player {
  Vector2 position;
  Vector2 velocity;
  Vector2 size;
  bool onGround;
  bool spawned;
} Player;

enum screen {
  MENU,
  GAME
};

enum screen screen = MENU;
static ArenaMap loadedMap = {0};
static Player player = {0};
static char mapLoadError[512] = "";

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

static void resetplayer(Player *player, const ArenaMap *map) {
  Vector2 spawnpoint = {(float)map->width * 0.5f, (float)map->height * 0.5f};

  if (map->spawnpointCount > 0) {
    spawnpoint = map->spawnpoints[0];
  }

  player->position = spawnpoint;
  player->velocity = (Vector2){0.0f, 0.0f};
  player->size = (Vector2){PLAYER_WIDTH, PLAYER_HEIGHT};
  player->onGround = false;
  player->spawned = true;
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

static void updateplayer(Player *player, const ArenaMap *map, float dt) {
  if (!player->spawned) {
    resetplayer(player, map);
  }

  float move = 0.0f;

  if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
    move -= 1.0f;
  }
  if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
    move += 1.0f;
  }

  player->velocity.x = move * PLAYER_SPEED;

  if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) ||
       IsKeyPressed(KEY_UP)) &&
      player->onGround) {
    player->velocity.y = -PLAYER_JUMP_SPEED;
    player->onGround = false;
  }

  player->position.x += player->velocity.x * dt;
  resolveplayerhorizontal(player, map);

  player->velocity.y += PLAYER_GRAVITY * dt;
  player->position.y += player->velocity.y * dt;
  resolveplayervertical(player, map);
}

void drawmenu(const char *title, int currentWidth, int currentHeight) {
  Rectangle menuWindow = {currentWidth / 2.0f - 180,
                          currentHeight / 2.0f - 120, 360, 240};
  Rectangle playButton = {currentWidth / 2.0f - 70,
                          currentHeight / 2.0f + 30, 140, 50};
  Vector2 mouse = GetMousePosition();
  bool playHovered = CheckCollisionPointRec(mouse, playButton);

  if (playHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    screen = GAME;
  }

  ClearBackground(DARKGRAY);
  DrawRectangleRec(menuWindow, RAYWHITE);
  DrawRectangleLinesEx(menuWindow, 2, BLACK);

  DrawText(title, currentWidth / 2 - MeasureText(title, 40) / 2,
           currentHeight / 2 - 70, 40, BLACK);
  DrawRectangleRec(playButton, playHovered ? LIGHTGRAY : GRAY);
  DrawRectangleLinesEx(playButton, 2, BLACK);
  DrawText("PLAY", currentWidth / 2 - MeasureText("PLAY", 28) / 2,
           currentHeight / 2 + 42, 28, BLACK);
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
                       int screenWidth, int screenHeight) {
  Rectangle bounds = rectfromcenter(player->position, player->size);
  Vector2 topLeft =
      maptoscreen((Vector2){bounds.x, bounds.y}, map, screenWidth,
                  screenHeight);
  Vector2 bottomRight =
      maptoscreen((Vector2){bounds.x + bounds.width, bounds.y + bounds.height},
                  map, screenWidth, screenHeight);
  Rectangle screenRect = {topLeft.x, topLeft.y, bottomRight.x - topLeft.x,
                          bottomRight.y - topLeft.y};

  DrawRectangleRec(screenRect, BLUE);
  DrawRectangleLinesEx(screenRect, 2.0f, (Color){12, 45, 120, 255});
}

void drawgame(int currentWidth, int currentHeight) {
  ClearBackground((Color){28, 34, 42, 255});

  if (!loadedMap.loaded) {
    DrawText(mapLoadError[0] != '\0' ? mapLoadError : "No map loaded", 30, 30,
             22, RED);
    return;
  }

  for (int i = 0; i < loadedMap.quadCount; i++) {
    drawquad(&loadedMap.quads[i], &loadedMap, currentWidth, currentHeight);
  }

  for (int i = 0; i < loadedMap.spawnpointCount; i++) {
    drawspawnpoint(loadedMap.spawnpoints[i], &loadedMap, currentWidth,
                   currentHeight);
  }

  if (player.spawned) {
    drawplayer(&player, &loadedMap, currentWidth, currentHeight);
  }

  DrawText(loadedMap.name, 24, 24, 24, RAYWHITE);
}

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 450;
  const char *title = "BRAWLHALLA";

  InitWindow(screenWidth, screenHeight, title);

  char *mapText = LoadFileText(MAP_FILE_PATH);

  if (mapText == NULL) {
    snprintf(mapLoadError, sizeof(mapLoadError), "Could not load %.450s",
             MAP_FILE_PATH);
  } else {
    MapList maps = {0};
    char parseError[512] = "";

    if (parsemaplistjson(mapText, &maps, parseError, sizeof(parseError))) {
      if (maps.count > 0) {
        loadedMap = maps.maps[0];
        mapLoadError[0] = '\0';
        resetplayer(&player, &loadedMap);
      } else {
        snprintf(mapLoadError, sizeof(mapLoadError), "%.180s: No maps found",
                 MAP_FILE_PATH);
      }
    } else {
      snprintf(mapLoadError, sizeof(mapLoadError), "%.180s: %.300s",
               MAP_FILE_PATH,
               parseError[0] != '\0' ? parseError : "Invalid map JSON");
    }

    UnloadFileText(mapText);
  }

  int monitor = GetCurrentMonitor();
  SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
  ToggleFullscreen();

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    int currentWidth = GetScreenWidth();
    int currentHeight = GetScreenHeight();

    if (screen == GAME && loadedMap.loaded) {
      updateplayer(&player, &loadedMap, GetFrameTime());
    }

    BeginDrawing();

    switch (screen) {
    case MENU:
      drawmenu(title, currentWidth, currentHeight);
      break;

    case GAME:
      drawgame(currentWidth, currentHeight);
      break;
    }

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
