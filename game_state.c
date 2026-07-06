#include "game_state.h"

#include "map_loader.h"
#include "raylib.h"

#include <stdio.h>

#define MAP_FILE_PATH "mappe/mappe.json"
#define PLAYER_WIDTH 36.0f
#define PLAYER_HEIGHT 54.0f

GameState gGame = {0};

static Vector2 fallbackspawnpoint(const ArenaMap *map, int playerIndex,
                                  int playerCount) {
  float centerX = (float)map->width * 0.5f;
  float centerY = (float)map->height * 0.5f;
  float offset = ((float)playerIndex - ((float)playerCount - 1.0f) * 0.5f) *
                 PLAYER_WIDTH * 1.5f;

  return (Vector2){centerX + offset, centerY};
}

static void resetplayers(const ArenaMap *map) {
  gGame.playerCount = DEFAULT_PLAYER_COUNT;

  if (gGame.playerCount > MAX_PLAYERS) {
    gGame.playerCount = MAX_PLAYERS;
  }

  for (int i = 0; i < MAX_PLAYERS; i++) {
    gGame.players[i] = (Player){0};
  }

  for (int i = 0; i < gGame.playerCount; i++) {
    Vector2 spawnpoint = i < map->spawnpointCount
                             ? map->spawnpoints[i]
                             : fallbackspawnpoint(map, i, gGame.playerCount);

    gGame.players[i].position = spawnpoint;
    gGame.players[i].velocity = (Vector2){0.0f, 0.0f};
    gGame.players[i].size = (Vector2){PLAYER_WIDTH, PLAYER_HEIGHT};
    gGame.players[i].onGround = false;
    gGame.players[i].spawned = true;
  }
}

void initGameState(void) {
  gGame = (GameState){0};
  gGame.screen = SCREEN_MENU;
  gGame.selectedMapIndex = -1;
  gGame.mapLoadError[0] = '\0';

  char *mapText = LoadFileText(MAP_FILE_PATH);

  if (mapText == NULL) {
    snprintf(gGame.mapLoadError, sizeof(gGame.mapLoadError),
             "Could not load %.450s", MAP_FILE_PATH);
    return;
  }

  MapList maps = {0};
  char parseError[512] = "";

  if (parsemaplistjson(mapText, &maps, parseError, sizeof(parseError))) {
    if (maps.count > 0) {
      gGame.availableMaps = maps;
      gGame.mapLoadError[0] = '\0';
    } else {
      snprintf(gGame.mapLoadError, sizeof(gGame.mapLoadError),
               "%.180s: No maps found", MAP_FILE_PATH);
    }
  } else {
    snprintf(gGame.mapLoadError, sizeof(gGame.mapLoadError),
             "%.180s: %.300s", MAP_FILE_PATH,
             parseError[0] != '\0' ? parseError : "Invalid map JSON");
  }

  UnloadFileText(mapText);
}

bool startGameWithMap(int index) {
  if (index < 0 || index >= gGame.availableMaps.count) {
    return false;
  }

  gGame.selectedMapIndex = index;
  gGame.currentMap = gGame.availableMaps.maps[index];
  resetplayers(&gGame.currentMap);
  gGame.screen = SCREEN_GAME;
  return true;
}

void returnToMenu(void) { gGame.screen = SCREEN_MENU; }
