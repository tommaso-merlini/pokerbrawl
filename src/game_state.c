#include "game_state.h"

#include "map_loader.h"
#include "raylib.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAP_FILE_PATH "mappe/mappe.json"
#define FALLBACK_SPAWN_SPACING 54.0f

GameState gGame = {0};

static const char *DEFAULT_CHARACTER_NAMES[] = {
    "Giordi", "Cianki", "Tommi", "Pippo", "Alessio",
};

static Character makecharacter(const char *name) {
  Character character = {0};
  snprintf(character.name, sizeof(character.name), "%s", name);
  return character;
}

static int findcharacterindexbyname(const char *name) {
  if (name == NULL || name[0] == '\0') {
    return -1;
  }

  for (int i = 0; i < gGame.characterCount; i++) {
    if (strcmp(gGame.availableCharacters[i].name, name) == 0) {
      return i;
    }
  }

  return -1;
}

static bool characternameexists(const char *name) {
  return findcharacterindexbyname(name) >= 0;
}

static Character defaultcharacter(void) {
  if (gGame.characterCount <= 0) {
    return (Character){0};
  }

  return gGame.availableCharacters[0];
}

static Character normalizedcharacter(Character character) {
  if (characternameexists(character.name)) {
    return character;
  }

  return defaultcharacter();
}

static int modeplayercount(GameMode mode) {
  switch (mode) {
  case GAME_MODE_1V1:
    return 2;
  }

  return DEFAULT_PLAYER_COUNT;
}

int getActivePlayerCount(void) {
  int playerCount = gGame.playerCount;

  if (playerCount <= 0) {
    playerCount = DEFAULT_PLAYER_COUNT;
  }
  if (playerCount > MAX_PLAYERS) {
    playerCount = MAX_PLAYERS;
  }

  return playerCount;
}

bool setPlayerCharacter(int playerIndex, int characterIndex) {
  if (playerIndex < 0 || playerIndex >= MAX_PLAYERS || characterIndex < 0 ||
      characterIndex >= gGame.characterCount) {
    return false;
  }

  gGame.players[playerIndex].character =
      gGame.availableCharacters[characterIndex];
  return true;
}

bool playerHasCharacter(int playerIndex, int characterIndex) {
  if (playerIndex < 0 || playerIndex >= MAX_PLAYERS || characterIndex < 0 ||
      characterIndex >= gGame.characterCount) {
    return false;
  }

  return findcharacterindexbyname(gGame.players[playerIndex].character.name) ==
         characterIndex;
}

void normalizeGameSelections(void) {
  if (gGame.selectedMode != GAME_MODE_1V1) {
    gGame.selectedMode = GAME_MODE_1V1;
  }
  gGame.playerCount = getActivePlayerCount();

  if (gGame.characterCount > 0) {
    for (int i = 0; i < gGame.playerCount; i++) {
      gGame.players[i].character =
          normalizedcharacter(gGame.players[i].character);
    }
  }

  if (gGame.availableMaps.count > 0 &&
      (gGame.selectedMapIndex < 0 ||
       gGame.selectedMapIndex >= gGame.availableMaps.count)) {
    gGame.selectedMapIndex = 0;
  }
}

static void initcharacters(void) {
  int defaultCharacterCount =
      (int)(sizeof(DEFAULT_CHARACTER_NAMES) /
            sizeof(DEFAULT_CHARACTER_NAMES[0]));

  gGame.characterCount = 0;

  for (int i = 0; i < defaultCharacterCount &&
                  gGame.characterCount < MAX_CHARACTERS;
       i++) {
    const char *name = DEFAULT_CHARACTER_NAMES[i];
    Character character = {0};

    if (name == NULL || name[0] == '\0') {
      continue;
    }

    character = makecharacter(name);
    if (character.name[0] == '\0' || characternameexists(character.name)) {
      continue;
    }

    gGame.availableCharacters[gGame.characterCount] = character;
    gGame.characterCount++;
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
  Character selectedCharacters[MAX_PLAYERS] = {0};

  for (int i = 0; i < MAX_PLAYERS; i++) {
    selectedCharacters[i] = normalizedcharacter(gGame.players[i].character);
  }

  gGame.playerCount = modeplayercount(gGame.selectedMode);

  if (gGame.playerCount > MAX_PLAYERS) {
    gGame.playerCount = MAX_PLAYERS;
  }

  for (int i = 0; i < MAX_PLAYERS; i++) {
    gGame.players[i] = (Player){0};
    gGame.players[i].character = selectedCharacters[i];
  }

  for (int i = 0; i < gGame.playerCount; i++) {
    Vector2 spawnpoint = i < map->spawnpointCount
                             ? map->spawnpoints[i]
                             : fallbackspawnpoint(map, i, gGame.playerCount);

    initPlayer(&gGame.players[i], spawnpoint, selectedCharacters[i]);
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
    gGame.players[i].character = defaultcharacter();
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
