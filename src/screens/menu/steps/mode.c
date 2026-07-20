#include "../menu_internal.h"

#include "../../../ui/ui.h"

static const char *modeLabel(GameMode mode) {
  switch (mode) {
  case GAME_MODE_1V1:
    return "1 VS 1";
  }

  return "";
}

void menuModeUpdate(GameState *game, const KeyboardState *keyboard,
                    MenuLayout layout) {
  Rectangle card = menuModeCard(layout.content);

  if (uiWasClicked(keyboard, card)) {
    game->selectedMode = GAME_MODE_1V1;
  }
}

void menuModeDraw(const GameState *game, MenuLayout layout) {
  Rectangle card = menuModeCard(layout.content);
  bool hovered = CheckCollisionPointRec(game->ui.pointer, card);

  uiDrawCard(modeLabel(game->selectedMode), card, true, hovered, true, 26);
}
