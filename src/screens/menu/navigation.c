#include "menu_internal.h"

static int wrappedIndex(int index, int count) {
  if (count <= 0) {
    return -1;
  }
  index %= count;
  return index < 0 ? index + count : index;
}

static int moveGridIndex(int index, int count, int columns,
                         UiNavigationCommand command) {
  if (count <= 0 || columns <= 0) {
    return -1;
  }

  index = wrappedIndex(index, count);
  if (command == UI_NAVIGATION_LEFT) {
    return wrappedIndex(index - 1, count);
  }
  if (command == UI_NAVIGATION_RIGHT) {
    return wrappedIndex(index + 1, count);
  }

  int column = index % columns;
  if (command == UI_NAVIGATION_DOWN) {
    int target = index + columns;
    if (target < count) {
      return target;
    }
    return column < count ? column : count - 1;
  }
  if (command == UI_NAVIGATION_UP) {
    int target = index - columns;
    if (target >= 0) {
      return target;
    }
    target = ((count - 1) / columns) * columns + column;
    while (target >= count) {
      target -= columns;
    }
    return target;
  }
  return index;
}

static bool isDirectionalCommand(UiNavigationCommand command) {
  return command == UI_NAVIGATION_UP || command == UI_NAVIGATION_RIGHT ||
         command == UI_NAVIGATION_DOWN || command == UI_NAVIGATION_LEFT;
}

static void sendMenuCommand(void *context, int playerIndex,
                            UiNavigationCommand command) {
  GameState *game = context;
  if (game == NULL || game->screen != SCREEN_MENU) {
    return;
  }
  if (game->menu.navigationTransitionHandled) {
    return;
  }

  if (command == UI_NAVIGATION_CONFIRM) {
    GameScreen previousScreen = game->screen;
    MenuStep previousStep = game->menu.step;
    menuAdvance(game);
    game->menu.navigationTransitionHandled =
        game->screen != previousScreen || game->menu.step != previousStep;
    return;
  }
  if (command == UI_NAVIGATION_BACK) {
    MenuStep previousStep = game->menu.step;
    menuPrevious(game);
    game->menu.navigationTransitionHandled = game->menu.step != previousStep;
    return;
  }
  if (!isDirectionalCommand(command)) {
    return;
  }

  MenuLayout layout = menuGetLayout(GetScreenWidth(), GetScreenHeight());
  if (game->menu.step == MENU_STEP_CHARACTERS) {
    if (playerIndex >= getActivePlayerCount(game)) {
      return;
    }
    int current = getPlayerCharacterIndex(game, playerIndex);
    int next = moveGridIndex(
        current, game->characterCount,
        menuCharacterColumns(layout.content, game->characterCount), command);
    setPlayerCharacter(game, playerIndex, next);
  } else if (game->menu.step == MENU_STEP_MAPS) {
    game->selectedMapIndex =
        moveGridIndex(game->selectedMapIndex, game->availableMaps.count,
                      menuMapColumns(layout.content), command);
  }
}

UiNavigationTarget menuScreenNavigation(GameState *game) {
  return (UiNavigationTarget){.context = game, .send = sendMenuCommand};
}

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
