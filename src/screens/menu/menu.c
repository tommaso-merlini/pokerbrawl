#include "menu.h"

#include "menu_internal.h"

void menuScreenUpdate(GameState *game, const InputState *input) {
  normalizeGameSelections(game);
  MenuLayout layout = menuGetLayout(GetScreenWidth(), GetScreenHeight());

  if (input->backPressed || (game->menu.step != MENU_STEP_MODE &&
                             uiWasClicked(input, layout.backButton))) {
    menuPrevious(game);
  }
  if (input->confirmPressed || uiWasClicked(input, layout.nextButton)) {
    menuAdvance(game);
  }

  switch (game->menu.step) {
  case MENU_STEP_MODE:
    menuModeUpdate(game, input, layout);
    break;
  case MENU_STEP_CHARACTERS:
    menuCharactersUpdate(game, input, layout);
    break;
  case MENU_STEP_MAPS:
    menuMapsUpdate(game, input, layout);
    break;
  }
}

void menuScreenDraw(const GameState *game, int screenWidth, int screenHeight) {
  MenuLayout layout = menuGetLayout(screenWidth, screenHeight);
  menuDrawFrame(game, layout);

  switch (game->menu.step) {
  case MENU_STEP_MODE:
    menuModeDraw(game, layout);
    break;
  case MENU_STEP_CHARACTERS:
    menuCharactersDraw(game, layout);
    break;
  case MENU_STEP_MAPS:
    menuMapsDraw(game, layout);
    break;
  }
}
