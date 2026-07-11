#include "../menu_internal.h"

#include "../../../game_state.h"
#include "../../../ui.h"

void menuMapsUpdate(MenuLayout layout) {
  Vector2 mouse = GetMousePosition();

  for (int i = 0; i < gGame.availableMaps.count; i++) {
    Rectangle card = menuMapCard(layout.content, i);

    if (CheckCollisionPointRec(mouse, layout.content) && uiWasClicked(card)) {
      gGame.selectedMapIndex = i;
    }
  }
}

void menuMapsDraw(MenuLayout layout) {
  Vector2 mouse = GetMousePosition();

  if (gGame.availableMaps.count <= 0) {
    uiDrawCenteredText(gGame.mapLoadError[0] != '\0' ? gGame.mapLoadError
                                                     : "Nessuna mappa trovata",
                       layout.content, 22, 12, RED);
    return;
  }

  for (int i = 0; i < gGame.availableMaps.count; i++) {
    Rectangle card = menuMapCard(layout.content, i);
    bool selected = i == gGame.selectedMapIndex;
    bool hovered = CheckCollisionPointRec(mouse, layout.content) &&
                   CheckCollisionPointRec(mouse, card);

    uiDrawCard(gGame.availableMaps.maps[i].name, card, selected, hovered, true,
               20);
  }
}
