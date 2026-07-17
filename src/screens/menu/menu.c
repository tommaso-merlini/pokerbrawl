#include "menu.h"

#include "menu_internal.h"

static void sendKeyboardNavigation(const InputState *input,
                                   const UiNavigationTarget *target) {
  if (input->navigateUpPressed) {
    uiSendNavigation(target, 0, UI_NAVIGATION_UP);
  }
  if (input->navigateRightPressed) {
    uiSendNavigation(target, 0, UI_NAVIGATION_RIGHT);
  }
  if (input->navigateDownPressed) {
    uiSendNavigation(target, 0, UI_NAVIGATION_DOWN);
  }
  if (input->navigateLeftPressed) {
    uiSendNavigation(target, 0, UI_NAVIGATION_LEFT);
  }
  if (input->confirmPressed) {
    uiSendNavigation(target, 0, UI_NAVIGATION_CONFIRM);
  }
  if (input->backPressed) {
    uiSendNavigation(target, 0, UI_NAVIGATION_BACK);
  }
}

void menuScreenUpdate(GameState *game, const InputState *input,
                      const ControllerRegistry *controllers) {
  normalizeGameSelections(game);
  game->menu.navigationTransitionHandled = false;
  UiNavigationTarget navigation = menuScreenNavigation(game);
  controllerRegistrySendNavigation(controllers, &navigation);
  sendKeyboardNavigation(input, &navigation);

  if (game->screen != SCREEN_MENU || game->menu.navigationTransitionHandled) {
    return;
  }

  MenuLayout layout = menuGetLayout(GetScreenWidth(), GetScreenHeight());

  if (game->menu.step != MENU_STEP_MODE &&
      uiWasClicked(input, layout.backButton)) {
    menuPrevious(game);
  }
  if (uiWasClicked(input, layout.nextButton)) {
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

void menuScreenDraw(const GameState *game,
                    const PlayerSpriteAssets *spriteAssets,
                    const ControllerRegistry *controllers, int screenWidth,
                    int screenHeight) {
  MenuLayout layout = menuGetLayout(screenWidth, screenHeight);
  menuDrawFrame(game, layout);
  menuDrawControllers(controllers, layout);

  switch (game->menu.step) {
  case MENU_STEP_MODE:
    menuModeDraw(game, layout);
    break;
  case MENU_STEP_CHARACTERS:
    menuCharactersDraw(game, spriteAssets, layout);
    break;
  case MENU_STEP_MAPS:
    menuMapsDraw(game, layout);
    break;
  }
}
