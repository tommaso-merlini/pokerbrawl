#include "raylib.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAP_FILE_PATH "mappe/mappe.json"
#define MAX_MAP_NAME 64
#define MAX_QUAD_ID 64
#define MAX_MAP_QUADS 128
#define MAX_MAP_SPAWNPOINTS 16
#define SPAWNPOINT_SIZE 24.0f
#define PLAYER_WIDTH 36.0f
#define PLAYER_HEIGHT 54.0f
#define PLAYER_SPEED 320.0f
#define PLAYER_JUMP_SPEED 700.0f
#define PLAYER_GRAVITY 1800.0f

typedef struct MapQuad {
  char id[MAX_QUAD_ID];
  Vector2 points[4];
  bool collidable;
} MapQuad;

typedef struct ArenaMap {
  char name[MAX_MAP_NAME];
  int width;
  int height;
  MapQuad quads[MAX_MAP_QUADS];
  int quadCount;
  Vector2 spawnpoints[MAX_MAP_SPAWNPOINTS];
  int spawnpointCount;
  bool loaded;
} ArenaMap;

typedef struct Player {
  Vector2 position;
  Vector2 velocity;
  Vector2 size;
  bool onGround;
  bool spawned;
} Player;

typedef struct JsonReader {
  const char *at;
  char error[256];
} JsonReader;

enum screen {
  MENU,
  GAME
};

enum screen screen = MENU;
static ArenaMap loadedMap = {0};
static Player player = {0};
static char mapLoadError[512] = "";

static bool jsonfail(JsonReader *reader, const char *message) {
  if (reader->error[0] == '\0') {
    snprintf(reader->error, sizeof(reader->error), "%s", message);
  }

  return false;
}

static void jsonskipws(JsonReader *reader) {
  while (*reader->at != '\0' && isspace((unsigned char)*reader->at)) {
    reader->at++;
  }
}

static bool jsonconsume(JsonReader *reader, char expected) {
  jsonskipws(reader);

  if (*reader->at != expected) {
    return false;
  }

  reader->at++;
  return true;
}

static bool jsonexpect(JsonReader *reader, char expected) {
  if (jsonconsume(reader, expected)) {
    return true;
  }

  char message[64];
  snprintf(message, sizeof(message), "Expected '%c'", expected);
  return jsonfail(reader, message);
}

static bool jsonliteral(JsonReader *reader, const char *literal) {
  jsonskipws(reader);
  int length = (int)strlen(literal);

  if (strncmp(reader->at, literal, length) != 0) {
    return false;
  }

  reader->at += length;
  return true;
}

static bool jsonstring(JsonReader *reader, char *out, int outSize) {
  jsonskipws(reader);

  if (*reader->at != '"') {
    return jsonfail(reader, "Expected string");
  }

  reader->at++;
  int length = 0;

  while (*reader->at != '\0' && *reader->at != '"') {
    char value = *reader->at++;

    if (value == '\\') {
      char escape = *reader->at++;

      switch (escape) {
      case '"':
      case '\\':
      case '/':
        value = escape;
        break;
      case 'b':
        value = '\b';
        break;
      case 'f':
        value = '\f';
        break;
      case 'n':
        value = '\n';
        break;
      case 'r':
        value = '\r';
        break;
      case 't':
        value = '\t';
        break;
      case 'u':
        for (int i = 0; i < 4; i++) {
          if (reader->at[i] == '\0' ||
              !isxdigit((unsigned char)reader->at[i])) {
            return jsonfail(reader, "Invalid unicode escape");
          }
        }
        reader->at += 4;
        value = '?';
        break;
      default:
        return jsonfail(reader, "Invalid string escape");
      }
    }

    if (length < outSize - 1) {
      out[length++] = value;
    }
  }

  if (*reader->at != '"') {
    return jsonfail(reader, "Unterminated string");
  }

  reader->at++;
  out[length] = '\0';
  return true;
}

static bool jsonnumber(JsonReader *reader, float *out) {
  jsonskipws(reader);

  if (*reader->at != '-' && !isdigit((unsigned char)*reader->at)) {
    return jsonfail(reader, "Expected number");
  }

  char *end = NULL;
  float value = strtof(reader->at, &end);

  if (end == reader->at) {
    return jsonfail(reader, "Invalid number");
  }

  reader->at = end;
  *out = value;
  return true;
}

static bool jsonbool(JsonReader *reader, bool *out) {
  if (jsonliteral(reader, "true")) {
    *out = true;
    return true;
  }

  if (jsonliteral(reader, "false")) {
    *out = false;
    return true;
  }

  return jsonfail(reader, "Expected boolean");
}

static bool jsonskipvalue(JsonReader *reader);

static bool jsonskipobject(JsonReader *reader) {
  if (!jsonexpect(reader, '{')) {
    return false;
  }

  if (jsonconsume(reader, '}')) {
    return true;
  }

  while (true) {
    char key[64];

    if (!jsonstring(reader, key, sizeof(key)) || !jsonexpect(reader, ':') ||
        !jsonskipvalue(reader)) {
      return false;
    }

    if (jsonconsume(reader, '}')) {
      return true;
    }

    if (!jsonexpect(reader, ',')) {
      return false;
    }
  }
}

static bool jsonskiparray(JsonReader *reader) {
  if (!jsonexpect(reader, '[')) {
    return false;
  }

  if (jsonconsume(reader, ']')) {
    return true;
  }

  while (true) {
    if (!jsonskipvalue(reader)) {
      return false;
    }

    if (jsonconsume(reader, ']')) {
      return true;
    }

    if (!jsonexpect(reader, ',')) {
      return false;
    }
  }
}

static bool jsonskipvalue(JsonReader *reader) {
  jsonskipws(reader);

  switch (*reader->at) {
  case '{':
    return jsonskipobject(reader);
  case '[':
    return jsonskiparray(reader);
  case '"': {
    char ignored[64];
    return jsonstring(reader, ignored, sizeof(ignored));
  }
  case 't':
  case 'f': {
    bool ignored = false;
    return jsonbool(reader, &ignored);
  }
  case 'n':
    if (jsonliteral(reader, "null")) {
      return true;
    }
    break;
  default: {
    float ignored = 0.0f;
    return jsonnumber(reader, &ignored);
  }
  }

  return jsonfail(reader, "Expected value");
}

static bool parsepoint(JsonReader *reader, Vector2 *point) {
  float x = 0.0f;
  float y = 0.0f;

  if (!jsonexpect(reader, '[') || !jsonnumber(reader, &x) ||
      !jsonexpect(reader, ',') || !jsonnumber(reader, &y) ||
      !jsonexpect(reader, ']')) {
    return false;
  }

  point->x = x;
  point->y = y;
  return true;
}

static bool parsequadpoints(JsonReader *reader, Vector2 points[4]) {
  if (!jsonexpect(reader, '[')) {
    return false;
  }

  for (int i = 0; i < 4; i++) {
    if (i > 0 && !jsonexpect(reader, ',')) {
      return false;
    }

    if (!parsepoint(reader, &points[i])) {
      return false;
    }
  }

  return jsonexpect(reader, ']');
}

static bool parsequad(JsonReader *reader, MapQuad *quad) {
  memset(quad, 0, sizeof(*quad));
  bool hasPoints = false;

  if (!jsonexpect(reader, '{')) {
    return false;
  }

  if (jsonconsume(reader, '}')) {
    return jsonfail(reader, "Quad needs points");
  }

  while (true) {
    char key[64];

    if (!jsonstring(reader, key, sizeof(key)) || !jsonexpect(reader, ':')) {
      return false;
    }

    if (strcmp(key, "id") == 0) {
      if (!jsonstring(reader, quad->id, sizeof(quad->id))) {
        return false;
      }
    } else if (strcmp(key, "collidable") == 0) {
      if (!jsonbool(reader, &quad->collidable)) {
        return false;
      }
    } else if (strcmp(key, "points") == 0) {
      if (!parsequadpoints(reader, quad->points)) {
        return false;
      }
      hasPoints = true;
    } else if (!jsonskipvalue(reader)) {
      return false;
    }

    if (jsonconsume(reader, '}')) {
      break;
    }

    if (!jsonexpect(reader, ',')) {
      return false;
    }
  }

  if (!hasPoints) {
    return jsonfail(reader, "Quad needs points");
  }

  return true;
}

static bool parsequads(JsonReader *reader, ArenaMap *map) {
  if (!jsonexpect(reader, '[')) {
    return false;
  }

  if (jsonconsume(reader, ']')) {
    return true;
  }

  while (true) {
    if (map->quadCount < MAX_MAP_QUADS) {
      if (!parsequad(reader, &map->quads[map->quadCount])) {
        return false;
      }
      map->quadCount++;
    } else if (!jsonskipvalue(reader)) {
      return false;
    }

    if (jsonconsume(reader, ']')) {
      return true;
    }

    if (!jsonexpect(reader, ',')) {
      return false;
    }
  }
}

static bool parsespawnpoints(JsonReader *reader, ArenaMap *map) {
  if (!jsonexpect(reader, '[')) {
    return false;
  }

  if (jsonconsume(reader, ']')) {
    return true;
  }

  while (true) {
    if (map->spawnpointCount < MAX_MAP_SPAWNPOINTS) {
      if (!parsepoint(reader, &map->spawnpoints[map->spawnpointCount])) {
        return false;
      }
      map->spawnpointCount++;
    } else if (!jsonskipvalue(reader)) {
      return false;
    }

    if (jsonconsume(reader, ']')) {
      return true;
    }

    if (!jsonexpect(reader, ',')) {
      return false;
    }
  }
}

static bool parsemap(JsonReader *reader, ArenaMap *map) {
  memset(map, 0, sizeof(*map));
  snprintf(map->name, sizeof(map->name), "untitled");
  map->width = 800;
  map->height = 450;

  if (!jsonexpect(reader, '{')) {
    return false;
  }

  if (jsonconsume(reader, '}')) {
    map->loaded = true;
    return true;
  }

  while (true) {
    char key[64];

    if (!jsonstring(reader, key, sizeof(key)) || !jsonexpect(reader, ':')) {
      return false;
    }

    if (strcmp(key, "name") == 0) {
      if (!jsonstring(reader, map->name, sizeof(map->name))) {
        return false;
      }
    } else if (strcmp(key, "width") == 0) {
      float width = 0.0f;
      if (!jsonnumber(reader, &width)) {
        return false;
      }
      map->width = (int)width;
    } else if (strcmp(key, "height") == 0) {
      float height = 0.0f;
      if (!jsonnumber(reader, &height)) {
        return false;
      }
      map->height = (int)height;
    } else if (strcmp(key, "quads") == 0) {
      if (!parsequads(reader, map)) {
        return false;
      }
    } else if (strcmp(key, "spawnpoints") == 0) {
      if (!parsespawnpoints(reader, map)) {
        return false;
      }
    } else if (!jsonskipvalue(reader)) {
      return false;
    }

    if (jsonconsume(reader, '}')) {
      break;
    }

    if (!jsonexpect(reader, ',')) {
      return false;
    }
  }

  if (map->width <= 0 || map->height <= 0) {
    return jsonfail(reader, "Map width and height must be positive");
  }

  map->loaded = true;
  return true;
}

static bool parsemapsarray(JsonReader *reader, ArenaMap *map) {
  if (!jsonexpect(reader, '[')) {
    return false;
  }

  if (jsonconsume(reader, ']')) {
    return jsonfail(reader, "Maps array is empty");
  }

  if (!parsemap(reader, map)) {
    return false;
  }

  while (true) {
    if (jsonconsume(reader, ']')) {
      return true;
    }

    if (!jsonexpect(reader, ',') || !jsonskipvalue(reader)) {
      return false;
    }
  }
}

static bool parsemapfile(JsonReader *reader, ArenaMap *map) {
  bool foundMaps = false;

  if (!jsonexpect(reader, '{')) {
    return false;
  }

  if (jsonconsume(reader, '}')) {
    return jsonfail(reader, "Map file needs a maps array");
  }

  while (true) {
    char key[64];

    if (!jsonstring(reader, key, sizeof(key)) || !jsonexpect(reader, ':')) {
      return false;
    }

    if (strcmp(key, "maps") == 0) {
      if (!parsemapsarray(reader, map)) {
        return false;
      }
      foundMaps = true;
    } else if (!jsonskipvalue(reader)) {
      return false;
    }

    if (jsonconsume(reader, '}')) {
      break;
    }

    if (!jsonexpect(reader, ',')) {
      return false;
    }
  }

  if (!foundMaps) {
    return jsonfail(reader, "Map file needs a maps array");
  }

  return true;
}

static bool loadfirstmap(const char *fileName, ArenaMap *map) {
  char *text = LoadFileText(fileName);

  if (text == NULL) {
    snprintf(mapLoadError, sizeof(mapLoadError), "Could not load %.450s",
             fileName);
    return false;
  }

  JsonReader reader = {.at = text, .error = ""};
  bool ok = parsemapfile(&reader, map);

  if (ok) {
    jsonskipws(&reader);

    if (*reader.at != '\0') {
      ok = jsonfail(&reader, "Unexpected data after JSON root");
    }
  }

  if (!ok) {
    snprintf(mapLoadError, sizeof(mapLoadError), "%.180s: %.300s", fileName,
             reader.error[0] != '\0' ? reader.error : "Invalid map JSON");
  }

  UnloadFileText(text);
  return ok;
}

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

  if (loadfirstmap(MAP_FILE_PATH, &loadedMap)) {
    resetplayer(&player, &loadedMap);
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
