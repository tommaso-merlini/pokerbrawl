#include "game_state.h"

#include "../maps/map_loader.h"
#include "game_state_internal.h"
#include "raylib.h"

#include <stdio.h>

#define MAP_FILE_PATH "mappe/mappe.json"

static void loadMaps(GameState *game) {
  char *text = LoadFileText(MAP_FILE_PATH);
  if (text == NULL) {
    snprintf(game->mapLoadError, sizeof(game->mapLoadError),
             "Could not load %.450s", MAP_FILE_PATH);
    return;
  }

  char error[512] = "";
  if (parseMapListJson(text, &game->availableMaps, error, sizeof(error)) &&
      game->availableMaps.count > 0) {
    game->selectedMapIndex = 0;
  } else {
    snprintf(game->mapLoadError, sizeof(game->mapLoadError), "%.180s: %.300s",
             MAP_FILE_PATH, error[0] != '\0' ? error : "No maps found");
  }

  UnloadFileText(text);
}

void initGameState(GameState *game) {
  *game = (GameState){
      .screen = SCREEN_MENU,
      .menu.step = MENU_STEP_MODE,
      .selectedMode = GAME_MODE_1V1,
      .selectedMapIndex = -1,
      .playerCount = DEFAULT_PLAYER_COUNT,
  };

  initAvailableCharacters(game);
  for (int i = 0; i < MAX_PLAYERS; i++) {
    game->players[i].character = defaultCharacter(game);
  }
  loadMaps(game);
}

void returnToMenu(GameState *game) { game->screen = SCREEN_MENU; }
