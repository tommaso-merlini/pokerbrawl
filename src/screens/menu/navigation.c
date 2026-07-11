#include "menu_internal.h"

bool menuCanAdvance(const GameState *game) {
  if (game->menu.step == MENU_STEP_CHARACTERS) {
    return game->characterCount > 0;
  }
  if (game->menu.step == MENU_STEP_MAPS) {
    return game->selectedMapIndex >= 0 &&
           game->selectedMapIndex < game->availableMaps.count;
  }
  return true;
}

void menuAdvance(GameState *game) {
  if (!menuCanAdvance(game)) {
    return;
  }

  switch (game->menu.step) {
  case MENU_STEP_MODE:
    game->menu.step = MENU_STEP_CHARACTERS;
    break;
  case MENU_STEP_CHARACTERS:
    game->menu.step = MENU_STEP_MAPS;
    break;
  case MENU_STEP_MAPS:
    startGameWithMap(game, game->selectedMapIndex);
    break;
  }
}

void menuPrevious(GameState *game) {
  if (game->menu.step == MENU_STEP_CHARACTERS) {
    game->menu.step = MENU_STEP_MODE;
  } else if (game->menu.step == MENU_STEP_MAPS) {
    game->menu.step = MENU_STEP_CHARACTERS;
  }
}
