#include "menu.h"

#include "menu_internal.h"

static void sendKeyboardNavigation(const KeyboardState *keyboard,
                                   const UiNavigationTarget *target) {
  if (keyboard->navigateUpPressed) {
    uiSendNavigation(target, 0, UI_NAVIGATION_UP);
  }
  if (keyboard->navigateRightPressed) {
    uiSendNavigation(target, 0, UI_NAVIGATION_RIGHT);
  }
  if (keyboard->navigateDownPressed) {
    uiSendNavigation(target, 0, UI_NAVIGATION_DOWN);
  }
  if (keyboard->navigateLeftPressed) {
    uiSendNavigation(target, 0, UI_NAVIGATION_LEFT);
  }
  if (keyboard->confirmPressed) {
    uiSendNavigation(target, 0, UI_NAVIGATION_CONFIRM);
  }
  if (keyboard->backPressed) {
    uiSendNavigation(target, 0, UI_NAVIGATION_BACK);
  }
}

void menuScreenUpdate(GameState *game, const KeyboardState *keyboard,
                      const ControllerRegistry *controllers) {
  normalizeGameSelections(game);
  game->menu.navigationTransitionHandled = false;
  UiNavigationTarget navigation = menuScreenNavigation(game);
  controllerRegistrySendNavigation(controllers, &navigation);
  sendKeyboardNavigation(keyboard, &navigation);

  if (game->screen != SCREEN_MENU || game->menu.navigationTransitionHandled) {
    return;
  }

  MenuLayout layout = menuGetLayout(GetScreenWidth(), GetScreenHeight());

  if (game->menu.step != MENU_STEP_MODE &&
      uiWasClicked(keyboard, layout.backButton)) {
    menuPrevious(game);
  }
  if (uiWasClicked(keyboard, layout.nextButton)) {
    menuAdvance(game);
  }

  switch (game->menu.step) {
  case MENU_STEP_MODE:
    menuModeUpdate(game, keyboard, layout);
    break;
  case MENU_STEP_CHARACTERS:
    menuCharactersUpdate(game, keyboard, layout);
    break;
  case MENU_STEP_MAPS:
    menuMapsUpdate(game, keyboard, layout);
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
