#include "game_state.h"

#include "map_loader.h"
#include "raylib.h"

#include <stdio.h>

#define MAP_FILE_PATH "mappe/mappe.json"
#define PLAYER_WIDTH 36.0f
#define PLAYER_HEIGHT 54.0f

GameState gGame = {0};

static void resetplayer(const ArenaMap *map) {
  Vector2 spawnpoint = {(float)map->width * 0.5f, (float)map->height * 0.5f};

  if (map->spawnpointCount > 0) {
    spawnpoint = map->spawnpoints[0];
  }

  gGame.player.position = spawnpoint;
  gGame.player.velocity = (Vector2){0.0f, 0.0f};
  gGame.player.size = (Vector2){PLAYER_WIDTH, PLAYER_HEIGHT};
  gGame.player.onGround = false;
  gGame.player.spawned = true;
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
  resetplayer(&gGame.currentMap);
  gGame.screen = SCREEN_GAME;
  return true;
}

void returnToMenu(void) { gGame.screen = SCREEN_MENU; }
