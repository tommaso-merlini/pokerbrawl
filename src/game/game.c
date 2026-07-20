#include "game.h"

#include "../screens/game/game_screen.h"
#include "../screens/game_end/game_end.h"
#include "../screens/menu/menu.h"

void updateGame(GameState *game, const KeyboardState *keyboard,
                const ControllerRegistry *controllers, float dt) {
  game->ui.pointer = keyboard->pointer;
  if (keyboard->debugTogglePressed) {
    game->debugMode = !game->debugMode;
  }

  switch (game->screen) {
  case SCREEN_MENU:
    menuScreenUpdate(game, keyboard, controllers);
    break;
  case SCREEN_GAME:
    gameScreenUpdate(game, keyboard, controllers, dt);
    break;
  case SCREEN_GAME_END:
    gameEndScreenUpdate(game, keyboard);
    break;
  }
}

void drawGame(const GameState *game, const PlayerRenderer *playerRenderer,
              const MapRenderer *mapRenderer,
              const ControllerRegistry *controllers, int screenWidth,
              int screenHeight) {
  switch (game->screen) {
  case SCREEN_MENU:
    menuScreenDraw(game, &playerRenderer->assets, controllers, screenWidth,
                   screenHeight);
    break;
  case SCREEN_GAME:
    gameScreenDraw(game, playerRenderer, mapRenderer, screenWidth,
                   screenHeight);
    break;
  case SCREEN_GAME_END:
    gameEndScreenDraw(game, screenWidth, screenHeight);
    break;
  }
}
