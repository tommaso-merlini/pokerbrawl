#include "game.h"

#include "../screens/game/game_screen.h"
#include "../screens/game_end/game_end.h"
#include "../screens/menu/menu.h"

void updateGame(GameState *game, const InputState *input, float dt) {
  game->ui.pointer = input->pointer;
  if (input->debugTogglePressed) {
    game->debugMapBoxes = !game->debugMapBoxes;
  }

  switch (game->screen) {
  case SCREEN_MENU:
    menuScreenUpdate(game, input);
    break;
  case SCREEN_GAME:
    gameScreenUpdate(game, input, dt);
    break;
  case SCREEN_GAME_END:
    gameEndScreenUpdate(game, input);
    break;
  }
}

void drawGame(const GameState *game, const PlayerRenderer *playerRenderer,
              const MapRenderer *mapRenderer, int screenWidth,
              int screenHeight) {
  switch (game->screen) {
  case SCREEN_MENU:
    menuScreenDraw(game, screenWidth, screenHeight);
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
