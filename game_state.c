#include "game_state.h"

#include "map_loader.h"
#include "raylib.h"

#include <stdio.h>

#define MAP_FILE_PATH "mappe/mappe.json"
#define FALLBACK_SPAWN_SPACING 54.0f

GameState gGame = {0};

static const char *DEFAULT_CHARACTER_NAMES[] = {
    "Giordi", "Cianki", "Tommi", "Pippo", "Alessio",
};

static int modeplayercount(GameMode mode) {
  switch (mode) {
  case GAME_MODE_1V1:
    return 2;
  }

  return DEFAULT_PLAYER_COUNT;
}

static void initcharacters(void) {
  int defaultCharacterCount =
      (int)(sizeof(DEFAULT_CHARACTER_NAMES) /
            sizeof(DEFAULT_CHARACTER_NAMES[0]));

  gGame.characterCount = defaultCharacterCount;
  if (gGame.characterCount > MAX_CHARACTERS) {
    gGame.characterCount = MAX_CHARACTERS;
  }

  for (int i = 0; i < gGame.characterCount; i++) {
    snprintf(gGame.availableCharacters[i].name,
             sizeof(gGame.availableCharacters[i].name), "%s",
             DEFAULT_CHARACTER_NAMES[i]);
  }
}

static Vector2 fallbackspawnpoint(const ArenaMap *map, int playerIndex,
                                  int playerCount) {
  float centerX = (float)map->width * 0.5f;
  float centerY = (float)map->height * 0.5f;
  float offset = ((float)playerIndex - ((float)playerCount - 1.0f) * 0.5f) *
                 FALLBACK_SPAWN_SPACING;

  return (Vector2){centerX + offset, centerY};
}

static void resetplayers(const ArenaMap *map) {
  gGame.playerCount = modeplayercount(gGame.selectedMode);

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

    initPlayer(&gGame.players[i], spawnpoint);
  }
}

void initGameState(void) {
  gGame = (GameState){0};
  gGame.screen = SCREEN_MENU;
  gGame.selectedMode = GAME_MODE_1V1;
  gGame.selectedMapIndex = -1;
  gGame.playerCount = modeplayercount(gGame.selectedMode);
  gGame.mapLoadError[0] = '\0';
  initcharacters();

  for (int i = 0; i < MAX_PLAYERS; i++) {
    gGame.playerSetups[i].selectedCharacterIndex = 0;
  }

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
      gGame.selectedMapIndex = 0;
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
