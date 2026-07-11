#include "game.h"

#include "../screens/game/game_screen.h"
#include "../screens/menu/menu.h"

void updateGame(GameState *game, const InputState *input, float dt) {
  game->ui.pointer = input->pointer;

  switch (game->screen) {
  case SCREEN_MENU:
    menuScreenUpdate(game, input);
    break;
  case SCREEN_GAME:
    gameScreenUpdate(game, input, dt);
    break;
  }
}

void drawGame(const GameState *game, const PlayerRenderer *renderer,
              int screenWidth, int screenHeight) {
  switch (game->screen) {
  case SCREEN_MENU:
    menuScreenDraw(game, screenWidth, screenHeight);
    break;
  case SCREEN_GAME:
    gameScreenDraw(game, renderer, screenWidth, screenHeight);
    break;
  }
}
