#include "../menu_internal.h"

#include "../../../game_state.h"
#include "../../../ui.h"

static const char *modelabel(GameMode mode) {
  switch (mode) {
  case GAME_MODE_1V1:
    return "1 VS 1";
  }

  return "";
}

void menuModeUpdate(MenuLayout layout) {
  Rectangle card = menuModeCard(layout.content);

  if (uiWasClicked(card)) {
    gGame.selectedMode = GAME_MODE_1V1;
  }
}

void menuModeDraw(MenuLayout layout) {
  Vector2 mouse = GetMousePosition();
  Rectangle card = menuModeCard(layout.content);
  bool hovered = CheckCollisionPointRec(mouse, card);

  uiDrawCard(modelabel(gGame.selectedMode), card, true, hovered, true, 26);
}
